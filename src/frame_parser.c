#include "headers.h"


int parse_element(cJSON *element){
	if(element->type == cJSON_Number){
		DLOG("\"%s\": %d\r\n",element->string,(int)element->valuedouble);
	} else if(element->type == cJSON_String){
		DLOG("\"%s\": \"%s\"\r\n",element->string,element->valuestring);
	} else if(element->type == cJSON_False){
		DLOG("\"%s\": false\r\n",element->string);
	} else if(element->type == cJSON_True){
		DLOG("\"%s\": true\r\n",element->string);
	} else if(element->type == cJSON_NULL){
		DLOG("\"%s\": NULL\r\n",element->string);
	} else {
		DLOG("Unknow type: %d\n", element->type);
	}
	return 0;
}

int parse_array_or_object(cJSON *tree){
	cJSON *b_element = NULL;
	cJSON_ArrayForEach(b_element, tree){
		if(b_element->type == cJSON_Array){
			parse_array_or_object(b_element);
		} else if(b_element->type == cJSON_Object){
			parse_array_or_object(b_element);
		} else {
			parse_element(b_element);
		}
	} 
	return 0;
}

int analog_input_parser(cJSON *tree, int to_fd){
	// int value = -1;
	// int pin = -1;
	// int port = -1;
	// int retval = -1;
	// char *dir = NULL;
	// double iepe_vref = 0.0;
    cJSON *item = NULL;
	char *outs;
	cJSON *root;
	
	//TODO: Stop previously ongoing AI task(if any)
	//channel selection of input as well as output
	if(NULL != (item = cJSON_GetObjectItem(tree, "ai_io_channel")))
		st_ain_sample.IO_channel  = (int)cJSON_GetNumberValue(item);
	
	//sensor type selection, for the ADC IC input channel selections
	if(NULL != (item = cJSON_GetObjectItem(tree, "ai_sensor_sel")))
		st_ain_sample.sensor_type  = (int)cJSON_GetNumberValue(item);

	// Power supply require for the Analog sensor, None, 420mA, 24V, or 12V
	if(NULL != (item = cJSON_GetObjectItem(tree, "ai_pwr_supply_src_sel")))
		st_ain_sample.ai_pwr_supply_src_sel  = (int)cJSON_GetNumberValue(item);

	if(NULL != (item = cJSON_GetObjectItem(tree, "ai_iepe_bias_sel")))
		st_ain_sample.IEPE_bias_type  = (int)cJSON_GetNumberValue(item);

	if(NULL != (item = cJSON_GetObjectItem(tree, "sampling_rate")))
		st_ain_sample.sampling_rate  = (int)cJSON_GetNumberValue(item);
	if(NULL != (item = cJSON_GetObjectItem(tree, "sample_count")))
		st_ain_sample.samp_cnt  = (int)cJSON_GetNumberValue(item) + 1;
	if(NULL != (item = cJSON_GetObjectItem(tree, "scan_period")))
		st_ain_sample.scan_period  = (int)cJSON_GetNumberValue(cJSON_GetObjectItem(tree, "scan_period"));
	// if(NULL != (item = cJSON_GetObjectItem(tree, "iepe_vref"))){
	// 	st_ain_sample.IEPE_Vref  = (double)cJSON_GetNumberValue(item);
	// 	set_tdc_vref(st_ain_sample.IEPE_Vref); //REVIEW: check if it is needed
	// }
	
	if(NULL != (item = cJSON_GetObjectItem(tree, "raw_data_req"))){
		if(item->type == cJSON_False){
			st_ain_sample.send_raw_samples = 0;
		} else if(item->type == cJSON_True) {
			st_ain_sample.send_raw_samples = 1;
		} else {
			DLOG("Invalid send_raw_samples type\r\n");
		}
	}

	if(NULL != (item = cJSON_GetObjectItem(tree, "show_raw_samples"))){
		if(item->type == cJSON_False){
			st_ain_sample.show_raw_samples = 0;
		} else if(item->type == cJSON_True) {
			st_ain_sample.show_raw_samples = 1;
		} else {
			DLOG("Invalid show_raw_samples type\r\n");
		}
	}

	
	DLOG("fs: %d\r\n", st_ain_sample.sampling_rate);
	DLOG("sample cnt: %d\r\n", st_ain_sample.samp_cnt);
	DLOG("send_raw_samples: %d\r\n", st_ain_sample.send_raw_samples);
	DLOG("scan_period: %d\r\n", st_ain_sample.scan_period);
	DLOG("iepe_vref: %f\r\n", st_ain_sample.IEPE_Vref);
	DLOG("sensor_type: %d\r\n", st_ain_sample.sensor_type);
	DLOG("channel: %d\r\n", st_ain_sample.IO_channel);

	st_ain_sample.request_tcp = true;
	ain_periodic_callback(NULL, &st_ain_sample);

	DLOG("[TCP] Min = %d, Max = %d, Avg = %d, delta = %d\n", st_ain_sample.min_samp, st_ain_sample.max_samp, st_ain_sample.avg_cnt, st_ain_sample.max_samp - st_ain_sample.min_samp);



	/* create root node and array */
	root = cJSON_CreateObject();

	/* add elements root object */
	if(NULL != (item = cJSON_GetObjectItem(tree, "token"))){
		cJSON_AddItemToObject(root, "token",cJSON_CreateNumber(cJSON_GetNumberValue(item)));
	}
	cJSON_AddItemToObject(root, "peripheral", cJSON_CreateString(cJSON_GetStringValue(cJSON_GetObjectItem(tree, "peripheral"))));
	cJSON_AddItemToObject(root, "response", cJSON_CreateString("ACK"));
	cJSON_AddItemToObject(root, "ai_sensor_sel", cJSON_CreateNumber(st_ain_sample.sensor_type));
	cJSON_AddItemToObject(root, "sample_count", cJSON_CreateNumber(st_ain_sample.samp_cnt));
	cJSON_AddItemToObject(root, "ai_io_channel", cJSON_CreateNumber(st_ain_sample.IO_channel));
	cJSON_AddItemToObject(root, "min", cJSON_CreateNumber(st_ain_sample.min_samp));
	cJSON_AddItemToObject(root, "max", cJSON_CreateNumber(st_ain_sample.max_samp));
	cJSON_AddItemToObject(root, "avg", cJSON_CreateNumber(st_ain_sample.avg_cnt));
	cJSON_AddItemToObject(root, "avg", cJSON_CreateNumber(getRandom(100,1500)));
	cJSON_AddItemToObject(root, "delta", cJSON_CreateNumber(st_ain_sample.max_samp - st_ain_sample.min_samp));
	char ts[30];
	getCurrentTimeMsStr(ts);
	cJSON_AddItemToObject(root, "timestamp", cJSON_CreateString(ts));
	//TODO:: add raw data in the response frames
	
	/* print everything */
	outs = cJSON_PrintUnformatted(root);
	send_to_clients(to_fd, outs, strlen(outs));
	DLOG("payload[%d]%s\n",(int)strlen(outs), outs);
	free(outs);

	/* free all objects under root and root itself */
	cJSON_Delete(root);
	return 0;
}


int gpio_parser(cJSON *tree, int to_fd){
	int value = -1;
	int pin = -1;
	int port = -1;
	int retval = -1;
	char *dir = NULL;
    cJSON *item = NULL;
	char *outs;
	cJSON *root;

	if(NULL != (item = cJSON_GetObjectItem(tree, "dir")))
		dir  = cJSON_GetStringValue(item);
	if(NULL != (item = cJSON_GetObjectItem(tree, "value")))
		value  = (int)cJSON_GetNumberValue(cJSON_GetObjectItem(tree, "value"));
	if(NULL != (item = cJSON_GetObjectItem(tree, "port")))
		port  = (int)cJSON_GetNumberValue(cJSON_GetObjectItem(tree, "port"));
	if(NULL != (item = cJSON_GetObjectItem(tree, "pin")))
		pin  = (int)cJSON_GetNumberValue(cJSON_GetObjectItem(tree, "pin"));
	
	DLOG("Port: %d-%d value: %d, dir: %s\r\n", port, pin, value, dir);
	if(dir != NULL && value != -1 && pin != -1 && port != -1 ){
		DLOG("[TCP] parsing; found valid parameters for GPIO\r\n");
		int gpio = IMX_GPIO_NR(port, pin);
		DLOG("Selected GPIO Pin: %d\n\r", gpio);
		retval = gpio_write_read(gpio, dir, value);
		/*Error Handling*/
		if (retval == 0) {
			DLOG("GPIO low\n\r");
		} else {
			DLOG("GPIO High\n\r");
		}
		//Send the response to the application whether the give steps are succeeded
	} else {
		DLOG("[TCP] parsing; invalid parameters for GPIO\r\n");
	}

	//response preparation
	/* create root node and array */
	root = cJSON_CreateObject();

	/* add elements root object */
	if(NULL != (item = cJSON_GetObjectItem(tree, "token"))){
		cJSON_AddItemToObject(root, "token",cJSON_CreateNumber(cJSON_GetNumberValue(item)));
	}

	cJSON_AddItemToObject(root, "peripheral", cJSON_CreateString(cJSON_GetStringValue(cJSON_GetObjectItem(tree, "peripheral"))));
	cJSON_AddItemToObject(root, "status", cJSON_CreateString("Success"));
	cJSON_AddItemToObject(root, "response", cJSON_CreateString("ACK"));

	cJSON_AddItemToObject(root, "GPIO_status", cJSON_CreateNumber(retval));
	cJSON_AddItemToObject(root, "port", cJSON_CreateNumber(port));
	cJSON_AddItemToObject(root, "pin", cJSON_CreateNumber(pin));
	cJSON_AddItemToObject(root, "value", cJSON_CreateNumber(value));
	char ts[30];
	getCurrentTimeMsStr(ts);
	cJSON_AddItemToObject(root, "timestamp", cJSON_CreateString(ts));
	
	/* print everything */
	outs = cJSON_PrintUnformatted(root);
	send_to_clients(to_fd, outs, strlen(outs));
	DLOG("payload[%d]%s\n",(int)strlen(outs), outs);
	free(outs);

	/* free all objects under root and root itself */
	cJSON_Delete(root);
	return 0;
}


int relay_parser(cJSON *tree, int to_fd){
	int32_t relay = -1;
	int32_t state = -1;
	int32_t operation = -1;
	uint32_t retval = -1;
	// char *dir = NULL;
    cJSON *item = NULL;
	char *outs;
	cJSON *root;

	if(NULL != (item = cJSON_GetObjectItem(tree, "relay_pos"))){
		relay  = (int)cJSON_GetNumberValue(item);
		operation = ((relay == 0xFF) ? RLY_BULK_OP_MODE : \
		(relay == 0xFE) ? RLY_INDVIDUAL_BULK_OP_MODE: RLY_SINGLE_OP_MODE);
	}

	if(NULL != (item = cJSON_GetObjectItem(tree, "state")))
		state  = (int)cJSON_GetNumberValue(item);
	
	//TODO: add additional checks to not to proceed further or not due to in correct parameters


	if(relay != -1 && state != -1 && operation != -1 ){
		DLOG("[TCP] parsing; found valid parameters for Relays\r\n");
		retval = relay_operation(relay, state, operation);

	} else {
		DLOG("[TCP] parsing; invalid parameters for Relay\r\n");
	}

	
	//response preparation
	/* create root node and array */
	root = cJSON_CreateObject();

	/* add elements root object */
	if(NULL != (item = cJSON_GetObjectItem(tree, "token"))){
		cJSON_AddItemToObject(root, "token",cJSON_CreateNumber(cJSON_GetNumberValue(item)));
	}
	cJSON_AddItemToObject(root, "peripheral", cJSON_CreateString(cJSON_GetStringValue(cJSON_GetObjectItem(tree, "peripheral"))));
	cJSON_AddItemToObject(root, "response", cJSON_CreateString("ACK"));

	cJSON_AddItemToObject(root, "relay_pos", cJSON_CreateNumber(relay));
	cJSON_AddItemToObject(root, "state", cJSON_CreateNumber(state));
	cJSON_AddItemToObject(root, "current_state", cJSON_CreateNumber(retval));
	char ts[30];
	getCurrentTimeMsStr(ts);
	cJSON_AddItemToObject(root, "timestamp", cJSON_CreateString(ts));
	
	/* print everything */
	outs = cJSON_PrintUnformatted(root);
	send_to_clients(to_fd, outs, strlen(outs));
	DLOG("payload[%d]%s\n", (int)strlen(outs), outs);
	free(outs);

	/* free all objects under root and root itself */
	cJSON_Delete(root);
	return 0;
}



int modbus_config_parser(cJSON *tree, int to_fd){
	int ser_channel = 1; //use defualt channel 1 in case there are no channel select field describe in the frame
	int mode_rs233_rs485 = -1;
	// int retval = -1;
    cJSON *item = NULL;
	char *outs;
	cJSON *root;

	if(NULL != (item = cJSON_GetObjectItem(tree, "ser_channel"))){
		ser_channel  = (int)cJSON_GetNumberValue(item);
		if((ser_channel > 3) && (ser_channel < 1)){
			ser_channel = 1;
		}
	}
	if(NULL != (item = cJSON_GetObjectItem(tree, "mode_rs233_rs485"))){
		mode_rs233_rs485  = (int)cJSON_GetNumberValue(item);
	}
	ser_enable_channel(mode_rs233_rs485, 0);
	

	//response preparation
	/* create root node and array */
	root = cJSON_CreateObject();

	/* add elements root object */
	if(NULL != (item = cJSON_GetObjectItem(tree, "token"))){
		cJSON_AddItemToObject(root, "token",cJSON_CreateNumber(cJSON_GetNumberValue(item)));
	}
	cJSON_AddItemToObject(root, "peripheral", cJSON_CreateString(cJSON_GetStringValue(cJSON_GetObjectItem(tree, "peripheral"))));
	cJSON_AddItemToObject(root, "response", cJSON_CreateString("ACK"));

	char ts[30];
	getCurrentTimeMsStr(ts);
	cJSON_AddItemToObject(root, "timestamp", cJSON_CreateString(ts));
	
	/* print everything */
	outs = cJSON_PrintUnformatted(root);
	send_to_clients(to_fd, outs, strlen(outs));
	DLOG("payload[%d]%s\n", (int)strlen(outs), outs);
	free(outs);

	/* free all objects under root and root itself */
	cJSON_Delete(root);
	return 0;
}




int digital_input_parser(cJSON *tree, int to_fd){
	// int retval = 0;
	// char *dir = NULL;
    cJSON *item = NULL;
	char *outs;
	cJSON *root;
	int di_channel = -1;


	//response preparation
	/* create root node and array */
	root = cJSON_CreateObject();

	/* add elements root object */
	if(NULL != (item = cJSON_GetObjectItem(tree, "token"))){
		cJSON_AddItemToObject(root, "token",cJSON_CreateNumber(cJSON_GetNumberValue(item)));
	}
	if(NULL != (item = cJSON_GetObjectItem(tree, "di_channel"))){
		di_channel = cJSON_GetNumberValue(item);
		cJSON_AddItemToObject(root, "di_channel", cJSON_CreateNumber(di_channel));
	} else {
		cJSON_AddItemToObject(root, "di_channel", cJSON_CreateNumber(255));
		di_channel = 255;

	}
	
	cJSON_AddItemToObject(root, "response", cJSON_CreateString("ACK"));

	cJSON_AddItemToObject(root, "peripheral", cJSON_CreateString(cJSON_GetStringValue(cJSON_GetObjectItem(tree, "peripheral"))));
	if(di_channel == 255 || di_channel == 1)cJSON_AddItemToObject(root, "DI_1", cJSON_CreateString((((digital_pin_in_read(config.digital_in_1_pin) & 0x01) == 0)? "OFF": "ON")));
	if(di_channel == 255 || di_channel == 2)cJSON_AddItemToObject(root, "DI_2", cJSON_CreateString((((digital_pin_in_read(config.digital_in_2_pin) & 0x01) == 0)? "OFF": "ON")));
	if(di_channel == 255 || di_channel == 3)cJSON_AddItemToObject(root, "DI_3", cJSON_CreateString((((digital_pin_in_read(config.digital_in_3_pin) & 0x01) == 0)? "OFF": "ON")));
	if(di_channel == 255 || di_channel == 4)cJSON_AddItemToObject(root, "DI_4", cJSON_CreateString((((digital_pin_in_read(config.digital_in_4_pin) & 0x01) == 0)? "OFF": "ON")));
	if(di_channel == 255 || di_channel == 5)cJSON_AddItemToObject(root, "DI_5", cJSON_CreateString((((digital_pin_in_read(config.digital_in_5_pin) & 0x01) == 0)? "OFF": "ON")));
	if(di_channel == 255 || di_channel == 6)cJSON_AddItemToObject(root, "DI_6", cJSON_CreateString((((digital_pin_in_read(config.digital_in_6_pin) & 0x01) == 0)? "OFF": "ON")));
	if(di_channel == 255 || di_channel == 7)cJSON_AddItemToObject(root, "DI_7", cJSON_CreateString((((digital_pin_in_read(config.digital_in_7_pin) & 0x01) == 0)? "OFF": "ON")));
	if(di_channel == 255 || di_channel == 8)cJSON_AddItemToObject(root, "DI_8", cJSON_CreateString((((digital_pin_in_read(config.digital_in_8_pin) & 0x01) == 0)? "OFF": "ON")));
	char ts[30];
	getCurrentTimeMsStr(ts);
	cJSON_AddItemToObject(root, "timestamp", cJSON_CreateString(ts));

	// retval = retval | ;
    // retval = retval | digital_pin_in_read(config.digital_in_2_pin) & 0x01) << 1;
    // retval = retval | digital_pin_in_read(config.digital_in_3_pin) & 0x01) << 2;
    // retval = retval | digital_pin_in_read(config.digital_in_4_pin) & 0x01) << 3;
    // retval = retval | digital_pin_in_read(config.digital_in_5_pin) & 0x01) << 4;
    // retval = retval | digital_pin_in_read(config.digital_in_6_pin) & 0x01) << 5;
    // retval = retval | digital_pin_in_read(config.digital_in_7_pin) & 0x01) << 6;
    // retval = retval | digital_pin_in_read(config.digital_in_8_pin) & 0x01) << 7;

	// cJSON_AddItemToObject(root, "current_state", cJSON_CreateNumber(retval));

	
	/* print everything */
	outs = cJSON_PrintUnformatted(root);
	send_to_clients(to_fd, outs, strlen(outs));
	DLOG("payload[%ld]%s\n",strlen(outs), outs);
	free(outs);

	/* free all objects under root and root itself */
	cJSON_Delete(root);
	return 0;
}


int rtd_parser(cJSON *tree, int to_fd){
	// int value = -1;
	// int pin = -1;
	// int port = -1;
	uint8_t channel = 0;
	// int retval = -1;
	// char *dir = NULL;
	// double iepe_vref = 0.0;
    cJSON *item = NULL;
	char *outs;
	cJSON *root;
	
	//TODO: Stop previously ongoing AI task(if any)
	if(NULL != (item = cJSON_GetObjectItem(tree, "rtd_channel_sel")))
		st_rtd_read.channel_scan  = (int)cJSON_GetNumberValue(item);
	if(NULL != (item = cJSON_GetObjectItem(tree, "sample_count")))
		st_rtd_read.rtd_sample_cnt  = (int)cJSON_GetNumberValue(item);
	if(NULL != (item = cJSON_GetObjectItem(tree, "scan_period")))
		st_rtd_read.scan_period  = (int)cJSON_GetNumberValue(item); //use to periodically call this read function and share the information to the server
	
	// if(NULL != (item = cJSON_GetObjectItem(tree, "raw_data_req"))){
	// 	if(item->type == cJSON_False){
	// 		st_ain_sample.send_raw_samples = 0;
	// 	} else if(item->type == cJSON_True) {
	// 		st_ain_sample.send_raw_samples = 1;
	// 	} else {
	// 		DLOG("Invalid send_raw_samples type\r\n");
	// 	}
	// }

	// if(NULL != (item = cJSON_GetObjectItem(tree, "show_raw_samples"))){
	// 	if(item->type == cJSON_False){
	// 		st_ain_sample.show_raw_samples = 0;
	// 	} else if(item->type == cJSON_True) {
	// 		st_ain_sample.show_raw_samples = 1;
	// 	} else {
	// 		DLOG("Invalid show_raw_samples type\r\n");
	// 	}
	// }

	
	DLOG("sample cnt: %d\r\n", st_rtd_read.rtd_sample_cnt);
	DLOG("scan_period: %d\r\n", st_rtd_read.scan_period);
	DLOG("RTD channel: %d\r\n", st_rtd_read.channel_scan);

	rtd_periodic_callback(NULL, &st_rtd_read);


	/* create root node and array */
	root = cJSON_CreateObject();

	/* add elements root object */
	if(NULL != (item = cJSON_GetObjectItem(tree, "token"))){
		cJSON_AddItemToObject(root, "token",cJSON_CreateNumber(cJSON_GetNumberValue(item)));
	}

    for (channel = 1; channel <= RTD_MAX_CHANNEL; channel++) {
		if(st_rtd_read.channel_scan == 0xff || st_rtd_read.channel_scan == channel){
			DLOG("min_adc[%d]: %d\r\n", channel, st_rtd_read.min_adc_count[channel - 1]);
			DLOG("max_adc[%d]: %d\r\n", channel, st_rtd_read.max_adc_count[channel - 1]);
			DLOG("avg_adc[%d]: %d\r\n", channel, st_rtd_read.avg_adc_count[channel - 1]);
			DLOG("delta_adc[%d]: %d\r\n", channel, st_rtd_read.max_adc_count[channel - 1] - st_rtd_read.min_adc_count[channel - 1]);
			DLOG("avg_temp[%d]: %f\r\n", channel, st_rtd_read.rtd_avg_temp[channel - 1]);
		}
	}
	if(st_rtd_read.channel_scan == 1) //as of now we are sending data of first channel only
	cJSON_AddItemToObject(root, "peripheral", cJSON_CreateString(cJSON_GetStringValue(cJSON_GetObjectItem(tree, "peripheral"))));
	cJSON_AddItemToObject(root, "response", cJSON_CreateString("ACK"));
	cJSON_AddItemToObject(root, "rtd_channel_sel", cJSON_CreateNumber(st_rtd_read.channel_scan));
	cJSON_AddItemToObject(root, "rtd_min_adc", cJSON_CreateNumber(st_rtd_read.min_adc_count[0]));
	cJSON_AddItemToObject(root, "rtd_max_adc", cJSON_CreateNumber(st_rtd_read.max_adc_count[0]));
	cJSON_AddItemToObject(root, "rtd_avg_adc", cJSON_CreateNumber(st_rtd_read.avg_adc_count[0]));
	cJSON_AddItemToObject(root, "rtd_delta_adc", cJSON_CreateNumber(st_rtd_read.max_adc_count[0] - st_rtd_read.min_adc_count[0]));
	cJSON_AddItemToObject(root, "rtd_avg_temp", cJSON_CreateNumber(st_rtd_read.rtd_avg_temp[0]));
	char ts[30];
	getCurrentTimeMsStr(ts);
	cJSON_AddItemToObject(root, "timestamp", cJSON_CreateString(ts));
	// cJSON_AddItemToObject(root, "sample_count", cJSON_CreateNumber(st_ain_sample.samp_cnt));
	// cJSON_AddItemToObject(root, "channel", cJSON_CreateNumber(st_ain_sample.IO_channel));
	// cJSON_AddItemToObject(root, "min", cJSON_CreateNumber(st_ain_sample.min_samp));
	// cJSON_AddItemToObject(root, "max", cJSON_CreateNumber(st_ain_sample.max_samp));
	// cJSON_AddItemToObject(root, "avg", cJSON_CreateNumber(st_ain_sample.avg_cnt));
	// cJSON_AddItemToObject(root, "delta", cJSON_CreateNumber(st_ain_sample.max_samp - st_ain_sample.min_samp));

	//TODO:: add raw data in the response frames
	
	/* print everything */
	outs = cJSON_PrintUnformatted(root);
	send_to_clients(to_fd, outs, strlen(outs));
	DLOG("Payload[%d] %s\n", (int)strlen(outs), outs);
	free(outs);

	/* free all objects under root and root itself */
	cJSON_Delete(root);
	return 0;

}


int request_frame_parser(char *pBuff, int len, int to_fd){

	cJSON *tree = NULL;
    cJSON *item = NULL;
    char *out = NULL;
	char *peripheral = NULL;

	//JSON validation and its parsing
	if(strchr(pBuff, '{') != NULL && strchr(pBuff, '}') != NULL){
		DLOG("Parsing the json data......: %s\r\n", pBuff);
		tree = cJSON_Parse(pBuff); //parsed the string parameter here so this can be used as the json object(structure)
    	out = cJSON_Print((const cJSON *)tree);
		free(out);

		//add parsing logic here 
		if(tree != NULL){
			//TODO: check if other required fields are also part of frame
			if(cJSON_HasObjectItem(tree, "peripheral")){
				item = cJSON_GetObjectItem(tree, "peripheral");
				peripheral = cJSON_GetStringValue(item);
				DLOG("peripheral Type: %s\r\n", peripheral);
				
				//this if for testing 
				//parse_array_or_object(tree);

				if(strcmp(peripheral, "ANALOG_INPUT") == 0){
					//parse the required field for the analog sensor
					DLOG("GOT AI command\r\n");
					analog_input_parser(tree, to_fd);
				} else if (strcmp(peripheral, "GPIO") == 0){
					DLOG("GOT GPIO command\r\n");
					gpio_parser(tree, to_fd);
				} else if (strcmp(peripheral, "RELAY") == 0){
					DLOG("GOT RLY command\r\n");
					relay_parser(tree, to_fd);
				} else if (strcmp(peripheral, "DI") == 0){
					DLOG("GOT DI command\r\n");
					digital_input_parser(tree, to_fd);
				} else if (strcmp(peripheral, "RTD") == 0){
					DLOG("GOT RTD command\r\n");
					rtd_parser(tree, to_fd);
				}  else if (strcmp(peripheral, "Modbus_conf") == 0){
					DLOG("GOT Modbus_conf command\r\n");
					modbus_config_parser(tree, to_fd);
				} else {
					DLOG("Invalid sensor type\r\n");
				}
			} else {
				DLOG("Not found peripheral key in received JSON\n\r");
				//TODO: print JSON if needed
			}

		}
		cJSON_Delete(tree);
	} else {
		DLOG("Not a JSON frame, Parsing is not supported yet.....\n");
		return 0;
	}
	return 0;
}
// // IPv4 demo of inet_ntop() and inet_pton()

// struct sockaddr_in sa;
// char str[INET_ADDRSTRLEN];

// // store this IP address in sa:
// inet_pton(AF_INET, "192.0.2.33", &(sa.sin_addr));

// // now get it back and print it
// inet_ntop(AF_INET, &(sa.sin_addr), str, INET_ADDRSTRLEN);

// DLOG("%s\n", str); // prints "192.0.2.33"

