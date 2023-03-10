/*******************************************************************************
 * Copyright (c) 2012, 2020 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * and Eclipse Distribution License v1.0 which accompany this distribution. 
 *
 * The Eclipse Public License is available at 
 *   https://www.eclipse.org/legal/epl-2.0/
 * and the Eclipse Distribution License is available at 
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Ian Craggs - initial contribution
 *    Fakhruddin Khan - added support for publish subscribe and integrate with other application
 * 
 * 
 * 
 *  - create client variable
 *  - create and set the variable in the client 
 *  - connect to MQTT
 *  - on connection subsribe to the required topics
 *  - periodically publish topics
 *
 *   Note: handling of each failure events need to be taken care, as of now we have just added debugs in it.
 *
 *******************************************************************************/
#include "headers.h"

/**
 * NOTE:  Steps taken to avoid disconnection issue
 * 
 * 1. check why we are sending the handshake though there are no connection
 * 2. reduce the max connection retry time to 5/10 seconds so that intermediate connection restore issues can be avoided
 * 3. subscribed before sending handshake in case there are some issue related to connectivity
 * 4. check if session clear flag can help here in this case
 * 5. also check if QoS class of MQTT can help as well
 * 6. at the end try to compare if there is any issue related to rabitMQ and compare it with different brokers.
 * 
 * //FINDINGS: 
 * isAuthenticate is reset in onconnlost callback and only after it receives DATE and time frame from Server
 * 	and if it is reset then
 * 	- No heartbeat will be sent
 * 	- It wont process any frames in fnActOnFrame() i.e Rx are not processed
 */

#if !defined(_WIN32)
#include <unistd.h>
#else
#include <windows.h>
#endif

#if defined(_WRS_KERNEL)
#include <OsWrapper.h>
#endif

int subscribed = 0;
int finished = 0;
int isMqttConnnected = 0;
char isSentFlag = 0;
char isPublishFailFlag = 0;
char ucSendFailCount = 0;
int iConnectFailCount = 0;

uint8_t isReconnectHandshake = 0;
uint8_t bPrintSepration;
volatile uint8_t isAuthenticate = 0;

// int myID = 25;
typedef void onMessage(char *, int , char *, int );
	
onMessage* onMessage_cb = NULL;	

uint32_t uiConsecutiveTxFailCnt = 0;
MQTTAsync client;

// struct pubsub_opts opts =
// {
// 	1, 0, 0, 0, "\n", 100,  	/* debug/app options */
// 	NULL, NULL, 1, 0, 0, /* message options */
// 	MQTTVERSION_DEFAULT, NULL, "mqtt_tls", 0, 0, "test_25", "timepass", "localhost", "1883", ADDRESS, 30, /* MQTT options */
// 	NULL, NULL, 0, 0, /* will options */
// 	0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, /* TLS options */
// 	0, {NULL, NULL}, /* MQTT V5 options */
// };




MQTTAsync_disconnectOptions disc_opts = MQTTAsync_disconnectOptions_initializer;
MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;
MQTTAsync_SSLOptions ssl_opts = MQTTAsync_SSLOptions_initializer;
MQTTAsync_createOptions create_opts = MQTTAsync_createOptions_initializer;
void onConnect(void* context, MQTTAsync_successData* response);
void onConnectFailure(void* context, MQTTAsync_failureData* response);


void connlost(void *context, char *cause)
{
	MQTTAsync clientL = (MQTTAsync)context;
	MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;
	MQTTAsync_responseOptions res_opts = MQTTAsync_responseOptions_initializer;
	int rc;
	
    if (cause)
	    ILOG("[MQTT] Connection lost, cause: %s\n", cause);
	else 
		ILOG("[MQTT] Connection lost\n");

	isAuthenticate = RESET;
	isReconnectHandshake = SET;
}


int messageArrived(void *context, char *topicName, int topicLen, MQTTAsync_message *message)
{
    // DLOG("[MQTT] Message arrived\n");
    // DLOG("[MQTT]    topic: %s\n", topicName);
    // DLOG("[MQTT]    message: %.*s\n", message->payloadlen, (char*)message->payload);
    if(onMessage_cb){
        onMessage_cb(topicName, topicLen, (char*)message->payload, message->payloadlen);
    }
    
    MQTTAsync_freeMessage(&message);
    MQTTAsync_free(topicName);
    return 1;
}

void onDisconnectFailure(void* context, MQTTAsync_failureData* response)
{
	ILOG("[MQTT] Disconnect failed, rc %d\n", response->code);
	finished = 1;
}

void onDisconnect(void* context, MQTTAsync_successData* response)
{
	ILOG("[MQTT] Successful disconnection\n");
	MQTTAsync_init(onMessage_cb); //Check if this works if not then need to find other alternate //TODO: TLS
	finished = 1;
}

void onSendFailure(void* context, MQTTAsync_failureData* response)
{
	MQTTAsync clientL = (MQTTAsync)context;
	MQTTAsync_disconnectOptions opts = MQTTAsync_disconnectOptions_initializer;
	int rc;

	ILOG("[MQTT] Message send failed token %d error code %d\n", response->token, response->code);

	isSentFlag = 0; //TODO: TLS
	ucSendFailCount++;
	isPublishFailFlag = 1; //TODO: TLS
	uiConsecutiveTxFailCnt = uiConsecutiveTxFailCnt << 1; //reset lsb on failure //TODO: TLS


	//if consecutive send failed occured then we can add logic here to try to reconnect the MQTT broker //TODO: TLS
	if(uiConsecutiveTxFailCnt & 0x3F == 0){ //last 6 frame failed
		opts.onSuccess = onDisconnect;
		opts.onFailure = onDisconnectFailure;
		opts.context = clientL;
		
		ILOG("[MQTT] consecutive %d publish failed, disconecting\n", uiConsecutiveTxFailCnt);

		if ((rc = MQTTAsync_disconnect(clientL, &opts)) != MQTTASYNC_SUCCESS)
		{
			ILOG("[MQTT] Failed to start disconnect, return code %d\n", rc);
			//exit(EXIT_FAILURE); //This line causes to terminate code
		}
	}
}

//this function would be replace with the callback as the third arg in publish function
void onSendFailure5(void* context, MQTTAsync_failureData5* response)
{
	MQTTAsync clientL = (MQTTAsync)context;
	MQTTAsync_disconnectOptions opts = MQTTAsync_disconnectOptions_initializer;
	int rc;

	ILOG("Message send failed token %d error code %d\n", response->token, response->code);
	// opts.onSuccess = onDisconnect;
	// opts.onFailure = onDisconnectFailure;
	// opts.context = clientL;
	// if ((rc = MQTTAsync_disconnect(clientL, &opts)) != MQTTASYNC_SUCCESS)
	// {
	// 	ILOG("Failed to start disconnect, return code %d\n", rc);
	// 	exit(EXIT_FAILURE);
	// }
}
void onSend(void* context, MQTTAsync_successData* response)
{
	MQTTAsync clientL = (MQTTAsync)context;
	MQTTAsync_disconnectOptions opts = MQTTAsync_disconnectOptions_initializer;
	int rc;
	isSentFlag = 0; //TODO: TLS
	// isPublishFailFlag = 1; //TODO: TLS
	uiConsecutiveTxFailCnt = uiConsecutiveTxFailCnt << 1 | 0x01; //set one bit on success //TODO: TLS
	ILOG("[MQTT] snd token: %d mask: 0x%08X\n", response->token, uiConsecutiveTxFailCnt);

}
//this function would be replace with the callback as the second arg in publish function
void onSend5(void* context, MQTTAsync_successData5* response)
{
	MQTTAsync clientL = (MQTTAsync)context;
	MQTTAsync_disconnectOptions opts = MQTTAsync_disconnectOptions_initializer;
	int rc;

	ILOG("Message with token value %d delivery confirmed\n", response->token);
	
}

void onSubscribe(void* context, MQTTAsync_successData* response)
{
	ILOG("[MQTT] Subscribe succeeded\n");
	subscribed = 1;
	isMqttConnnected = 1; //Added here as we will start sending the packets after subscription
}

void onSubscribeFailure(void* context, MQTTAsync_failureData* response)
{
	ILOG("[MQTT] Subscribe failed, rc %d\n", response->code);
	finished = 1;
}

void onUnSubscribe(void* context, MQTTAsync_successData* response)
{
	ILOG("[MQTT] Unsubscribe succeeded\n");
	subscribed = 1;
}

void onUnSubscribeFailure(void* context, MQTTAsync_failureData* response)
{
	ILOG("[MQTT] Un Subscribe failed, rc %d\n", response->code);
	finished = 1;
}


void onConnectFailure(void* context, MQTTAsync_failureData* response)
{
	ILOG("[MQTT] Connect failed, rc %d\n", response ? response->code : 0);
	finished = 1;
    isMqttConnnected = 0;
	isReconnectHandshake = 1;
	iConnectFailCount++;

	if(iConnectFailCount >= config.maxConnFailCnt_reboot){
		//TODO: do neccessary things before reboot or stop all the process here
		ILOG("MQTT retry[%d] count: %d, System rebooting...\n", config.maxConnFailCnt_reboot,iConnectFailCount);
		system("reboot");   //TODO: this is not a correct way, we need a common function where we can call and update the reboot casue also need to do a safe reboot

	}
}

void onConnectFailure5(void* context, MQTTAsync_failureData5* response)
{
	int ret_val = -1;
	fprintf(stderr, "Connect failed, rc %s reason code %s\n",
		MQTTAsync_strerror(response->code),
		MQTTReasonCode_toString(response->reasonCode));
	isMqttConnnected = 0;
	isReconnectHandshake = 1;
	iConnectFailCount++;

	if(iConnectFailCount >= config.maxConnFailCnt_reboot){
		//TODO: do neccessary things before reboot or stop all the process here
		ILOG("MQTT retry count: %d, System rebooting...\n", iConnectFailCount);
		ret_val = system("sync;reboot");   //TODO: this is not a correct way, we need a common function where we can call and update the reboot casue also need to do a safe reboot

	}
	//MQTTAsync client = (MQTTAsync)context;
}

void onConnect(void* context, MQTTAsync_successData* response)
{
	MQTTAsync clientL = (MQTTAsync)context;
	MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
	MQTTAsync_responseOptions res_opts = MQTTAsync_responseOptions_initializer;
	int rc;

	ILOG("[MQTT] Successful connection\n");
    char rgcTopic[100] = {0};
    sprintf( rgcTopic, "%s%s", config.to_logger_topic_prefix,config.gateway_id ); 
	ILOG("[MQTT] unSubscribing to topic: %s\n", rgcTopic);
	
	opts.onSuccess = onUnSubscribe;
	opts.onFailure = onUnSubscribeFailure;
	opts.context = clientL;

	if ((rc = MQTTAsync_unsubscribe(clientL, rgcTopic, &opts)) != MQTTASYNC_SUCCESS)
	{
		ILOG("[MQTT] Failed to unsubscribe, return code %d\n", rc);
		finished = 1;
	}
	
	opts.onSuccess = onSubscribe;
	opts.onFailure = onSubscribeFailure;
	opts.context = clientL;
	ILOG("[MQTT] Subscribing to topic: %s\n", rgcTopic);


	if ((rc = MQTTAsync_subscribe(clientL, rgcTopic, QOS, &opts)) != MQTTASYNC_SUCCESS)
	{
		ILOG("[MQTT] Failed to start subscribe, return code %d\n", rc);
		finished = 1;
	}
    // isMqttConnnected = 1;
	iConnectFailCount = 0; //reset the counter after successful connnection with the broker
    
}

void onConnect5(void* context, MQTTAsync_successData5* response)
{
	MQTTAsync clientL = (MQTTAsync)context;
	MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
	int rc;

	ILOG("[MQTT5] Successful connection\n");
    char rgcTopic[100] = {0};
    sprintf(rgcTopic, "%s%s", config.to_logger_topic_prefix,config.gateway_id ); 
	// DLOG("Subscribing to topic %s\nfor clientL %s using QoS%d\n", rgcTopic, CLIENTID, QOS);
	opts.onSuccess = onSubscribe;
	opts.onFailure = onSubscribeFailure;
	opts.context = clientL;
	if ((rc = MQTTAsync_subscribe(clientL, rgcTopic, QOS, &opts)) != MQTTASYNC_SUCCESS)
	{
		ILOG("[MQTT5] Failed to subscribe, return code %d\n", rc);
		finished = 1;
	}
    // isMqttConnnected = 1;
    
}



void subscribeTopic(void)
{
	MQTTAsync_responseOptions resp_opts = MQTTAsync_responseOptions_initializer;
	int rc;

    char rgcTopic[100] = {0};
    sprintf( rgcTopic, "%s%s", config.to_logger_topic_prefix,config.gateway_id ); 
	ILOG("[MQTT] unSubscribing to topic: %s\n", rgcTopic);
	
	resp_opts.onSuccess = onUnSubscribe;
	resp_opts.onFailure = onUnSubscribeFailure;
	resp_opts.context = client;

	if ((rc = MQTTAsync_unsubscribe(client, rgcTopic, &resp_opts)) != MQTTASYNC_SUCCESS)
	{
		ILOG("[MQTT] Failed to unsubscribe, return code %d\n", rc);
		finished = 1;
	}
	
	resp_opts.onSuccess = onSubscribe;
	resp_opts.onFailure = onSubscribeFailure;
	resp_opts.context = client;
	ILOG("[MQTT] Subscribing to topic: %s\n", rgcTopic);

	if ((rc = MQTTAsync_subscribe(client, rgcTopic, QOS, &resp_opts)) != MQTTASYNC_SUCCESS)
	{
		ILOG("[MQTT] Failed to start subscribe, return code %d\n", rc);
		finished = 1;
	}
    // connected = 1;
	iConnectFailCount = 0; //reset the counter after successful connnection with the broker
    
}
static int onSSLError(const char *str, size_t len, void *context)
{
	MQTTAsync client = (MQTTAsync)context;
	return fprintf(stderr, "SSL error: %s\n", str);
}

static unsigned int onPSKAuth(  const char* hint, char* identity,
                                unsigned int max_identity_len,unsigned char* psk,
                                unsigned int max_psk_len, void* context)
{
	int psk_len;
	int k, n;

	int rc = 0;
	//TODO: need to add this feature later, to do so we need to add these parameters in config
	// struct pubsub_opts* opts = context;

	// /* DLOG("Trying TLS-PSK auth with hint: %s\n", hint);*/

	// if (opts->psk == NULL || opts->psk_identity == NULL)
	// {
	// 	/* DLOG("No PSK entered\n"); */
	// 	goto exit;
	// }

	// /* psk should be array of bytes. This is a quick and dirty way to
	//  * convert hex to bytes without input validation */
	// psk_len = (int)strlen(opts->psk) / 2;
	// if (psk_len > max_psk_len)
	// {
	// 	fprintf(stderr, "PSK too long\n");
	// 	goto exit;
	// }
	// for (k=0, n=0; k < psk_len; k++, n += 2)
	// {
	// 	sscanf(&opts->psk[n], "%2hhx", &psk[k]);
	// }

	// /* identity should be NULL terminated string */
	// strncpy(identity, opts->psk_identity, max_identity_len);
	// if (identity[max_identity_len - 1] != '\0')
	// {
	// 	fprintf(stderr, "Identity too long\n");
	// 	goto exit;
	// }

	// /* Function should return length of psk on success. */
	// rc = psk_len;

exit:
	return rc;
}

/**
 * @brief  this function will send the 
 * 
 * @param context - MQTT client object
 * @param payload 
 */
void publish(char *payload, int len)
{
	if(MQTTAsync_isConnected(client)){ // check if MQTT is connected and then only publish it

		
		MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
		MQTTAsync_message pubmsg = MQTTAsync_message_initializer;
		int rc;

		opts.onSuccess = onSend;
		opts.onFailure = onSendFailure;
		opts.context = client;

		//DLOG("[MQTT] publishing\n");
		pubmsg.payload = payload;
		pubmsg.payloadlen = (int)strlen(payload);
		pubmsg.qos = QOS;
		pubmsg.retained = 0;
		char rgcTopic[100] = "";
		sprintf(rgcTopic, "%s%s", config.to_server_topic_prefix,config.gateway_id ); 

		//DLOG("[MQTT] Publishing to %s\nfor client %s using QoS: %d\n", rgcTopic, CLIENTID, QOS);
		if ((rc = MQTTAsync_sendMessage(client, rgcTopic, &pubmsg, &opts)) != MQTTASYNC_SUCCESS)
		{
			ILOG("[MQTT] Failed to start sendMessage, return code %d\n", rc);
			//exit(EXIT_FAILURE);
		}
		//DLOG("DONE\n");
	} else {
		DLOG("[MQTT] publish, not connected to broker\r\n");
	}
}

void onMQTT_Message_cb(char *topicName, int topicLen, char *message, int payloadlen)
{
	ILOG("[MQTT] RCV t: %.*s, msg: %.*s\n",topicLen, topicName,payloadlen,message );
    // writeServerInQueue(message, payloadlen);
	request_frame_parser(message, payloadlen, -1);
}

int MQTTAsync_init(onMessage* MQTT_onMessage_cb)
{
	int rc;
    onMessage_cb = MQTT_onMessage_cb;
	create_opts.sendWhileDisconnected = 0; //REVIEW: it was set to 1 earlier just disable it to not to allow sending a packet to broker if it is disconnected

	//Set parameters here to connect to the MQTT broker via ssl/tls or no certificate
	//opts.MQTTVersion = MQTTVERSION_5; //enable this when required MQTT5
	
	ILOG("[MQTT] Config user: %s\r\n", config.username);
	ILOG("[MQTT] Config passwd: %s\r\n", config.password);
	ILOG("[MQTT] Config clientid: %s\r\n", config.clientid);
	ILOG("[MQTT] Config url: %s\r\n", config.mqtt_broker_url);

    if (config.MQTTVersion >= MQTTVERSION_5)
		create_opts.MQTTVersion = MQTTVERSION_5;
    if ((rc = MQTTAsync_createWithOptions(&client, config.mqtt_broker_url, config.clientid, MQTTCLIENT_PERSISTENCE_NONE, NULL, &create_opts)) != MQTTASYNC_SUCCESS)
	{
		if (!config.quiet)
			fprintf(stderr, "Failed to create client, return code: %s\n", MQTTAsync_strerror(rc));
		//exit(EXIT_FAILURE);
	}

    if (config.MQTTVersion == MQTTVERSION_5)
	{
		ILOG("[MQTT] version v5\r\n");
		MQTTAsync_connectOptions conn_opts5 = MQTTAsync_connectOptions_initializer5;
		conn_opts = conn_opts5;
		conn_opts.onSuccess5 = onConnect5;
		conn_opts.onFailure5 = onConnectFailure5;
		conn_opts.cleanstart = 1;
	}
	else
	{
		ILOG("[MQTT] version v3\r\n");
		conn_opts.onSuccess = onConnect;
		conn_opts.onFailure = onConnectFailure;
		conn_opts.cleansession = 0;
	}
	conn_opts.keepAliveInterval = config.MqttKeepalive;
	conn_opts.username = config.username;
	conn_opts.password = config.password;
	conn_opts.MQTTVersion = config.MQTTVersion;
	conn_opts.context = client;
	conn_opts.automaticReconnect = 1;
	conn_opts.minRetryInterval = 1;
	conn_opts.maxRetryInterval = 20;

	
	if (config.mqtt_broker_url && (strncmp(config.mqtt_broker_url, "ssl://", 6) == 0 ||
			strncmp(config.mqtt_broker_url, "wss://", 6) == 0))
	{
		ILOG("[MQTT] setting up the SSL options\n");
		if (config.insecure)
			ssl_opts.verify = 0;
		else
			ssl_opts.verify = 1;
		ssl_opts.CApath = config.capath;
		ssl_opts.keyStore = config.cert;
		ssl_opts.trustStore = config.cafile;
		ssl_opts.privateKey = config.key;
		ssl_opts.privateKeyPassword = config.keypass;
		ssl_opts.enabledCipherSuites = config.ciphers;
		ssl_opts.ssl_error_cb = onSSLError;
		ssl_opts.ssl_error_context = client;
		ssl_opts.ssl_psk_cb = onPSKAuth;
		ssl_opts.ssl_psk_context = &config;
		conn_opts.ssl = &ssl_opts;
	}

	if ((rc = MQTTAsync_setCallbacks(client, client, connlost, messageArrived , NULL)) != MQTTASYNC_SUCCESS)
	{
		ILOG("Failed to set callback, return code %d\n", rc);
		//exit(EXIT_FAILURE);
	}

	if ((rc = MQTTAsync_connect(client, &conn_opts)) != MQTTASYNC_SUCCESS)
	{
		ILOG("Failed to start connect, return code %d\n", rc);
		//exit(EXIT_FAILURE);
	}
    return rc;
}

int MQTTAsync_Deinit(MQTTAsync_onSuccess* onDisconnect, MQTTAsync_onFailure* onDisconnectFailure)
{
    // MQTTAsync clientL = (MQTTAsync)context;
	MQTTAsync_disconnectOptions opts = MQTTAsync_disconnectOptions_initializer;
	int rc;

	ILOG("Disconnecting MQTT\n");
	opts.onSuccess = onDisconnect;
	opts.onFailure = onDisconnectFailure;
	opts.context = client;
	if ((rc = MQTTAsync_disconnect(client, &opts)) != MQTTASYNC_SUCCESS)
	{
		ILOG("Failed to start disconnect, return code %d\n", rc);
		exit(EXIT_FAILURE);
	}

	return 1;
}
// int MQTTAsync_init(onMessage* MQTT_onMessage_cb)
// {
// 	MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;
// 	int rc;
//     onMessage_cb = MQTT_onMessage_cb;
// 	if ((rc = MQTTAsync_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL)) != MQTTASYNC_SUCCESS)
// 	{
// 		DLOG("[MQTT] Failed to create client object, return code %d\n", rc);
// 		//exit(EXIT_FAILURE);
// 	}

// 	if ((rc = MQTTAsync_setCallbacks(client, client, connlost, messageArrived, NULL)) != MQTTASYNC_SUCCESS)
// 	{
// 		DLOG("[MQTT] Failed to set callback, return code %d\n", rc);
// 		//exit(EXIT_FAILURE);
// 	}
// 	conn_opts.keepAliveInterval = opts.keepalive;
// 	conn_opts.cleansession = 1;
// 	conn_opts.onSuccess = onConnect;
// 	conn_opts.onFailure = onConnectFailure;
// 	conn_opts.context = client;
// 	conn_opts.username = "mqtt";
// 	conn_opts.password = "mqtt";

// 	if ((rc = MQTTAsync_connect(client, &conn_opts)) != MQTTASYNC_SUCCESS)
// 	{
// 		DLOG("[MQTT] Failed to start connect, return code %d\n", rc);
// 		//exit(EXIT_FAILURE);
// 	}
//     return rc;
// }

/**
 * @brief This function is used to check if there is any connectivity issue and take any action if required
 *        Call this function in continuous while loop in case of single thread in case of multi 
 *        threading call it in the main loop of the thread
 * 
 */
// void MQTTAsync_Periodic_Check(void){
//     if (!finished && isMqttConnnected){
//         if(isMqttConnnected){
//             //publish(PAYLOAD);
//             // DLOG("[MQTT] Publish here\r\n");
//         }
//     }
// }


void onMessage_app_cb(char *topicName, int topicLen, char *message, int payloadlen)
{
    ILOG("[MQTT] RCV t: %s, msg: %.*s\n",topicName,payloadlen,message );
    // DLOG("[MQTT]    topic: %s\n", topicName);
    // DLOG("[MQTT]    message: %.*s\n", payloadlen, message);
    
    // return 1;
}


// int main(int argc, char* argv[])
// {
//     if(MQTTASYNC_SUCCESS ==  MQTTAsync_init(onMessage_app_cb)){
//         DLOG("[MQTT] MQTT Init and isMqttConnnected!!!\r\n");
//     }
//     while (1){
//         #if defined(_WIN32)
// 			Sleep(100);
// 		#else
// 			usleep(1000000L); // 1 sec delay
// 		#endif
//         MQTTAsync_Periodic_Check();
//     }
//     MQTTAsync_destroy(&client);
// }
