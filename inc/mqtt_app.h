/**
 * @brief 
 * 
 */
#ifndef __MQTT_APP_H
#define __MQTT_APP_H
#include "MQTTAsync.h"
#define ADDRESS                    "tcp://43.225.161.182:1883" /* acevin server */ 
//#define ADDRESS                  "tcp://35.225.129.135:1883" /* Itanta server */
#define CLIENTID                   "LoggerID14"
#define TOPIC_LOGGER_PREFIX_       "server/"
#define TOPIC_SERVER_PREFIX_       "logger/"
#define PAYLOAD                     "{ \"id\": 26, \"data\": \"hello world!\" }"
#define QOS                         0
#define TIMEOUT                     10000L
#define RETRY_PUBLISH               3 //retry 3 times and if failed then need to reconnect the mqtt

extern int subscribed;
extern int finished ;
extern int isMqttConnnected;

typedef void onMessage(char *, int , char *, int );

extern void publish(char *payload, int len);
extern int MQTTAsync_init(onMessage* MQTT_onMessage_cb);
extern void MQTTAsync_Periodic_Check(void);
extern MQTTAsync client;

//Extern flags
extern uint8_t isReconnectHandshake;
extern uint8_t bPrintSepration;
extern volatile uint8_t isAuthenticate;
extern char isSentFlag;
extern char isPublishFailFlag;
extern char ucSendFailCount;

extern struct pubsub_opts opts;
extern void subscribeTopic(void);
extern uint32_t uiConsecutiveTxFailCnt;
void onMQTT_Message_cb(char *topicName, int topicLen, char *message, int payloadlen);


#endif // __MQTT_APP_H

