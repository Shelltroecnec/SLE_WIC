# Ultrafab-IQ API frame format(Between flow and application driver)


## List of API's
### Available API's in Low level drivers 
"peripheral": String
	Available options:	[
		"GPIO", 
		"ANALOG_INPUT",
		"RTD",
		"DI",
		"RELAY",
		"Modbus_conf"]

"method": 
	Available options: ["SET", "GET"]


Required fields for "GPIO" sensor type:
"port"	: Number, //1 - 5
"pin"	: Number, //0-31
"dir"	: string, //read/input/output
"value"	: Number, //0 - Low, 1 - High; only needed in case of output direction


Required fields for "AI" sensor type:
"ch"			: Channel number to be read
"sensor_type"	: Number, analog sensor type
"sampling_rate" : Number, 
"sample_count"	: Number, //samples required
"raw_data_req"	: Number, //raw data needed to be sent.  0 - No (default, no need of this field) 1 - Yes
"iepe_vref" 	: Number, //float value IEPE Vref voltage, only needed if sensor_type is VIBRATION_AC_BIAS VIBRATION_DC_BIAS
"scan_period"	: Number, //continuous scanning of this sensor until new commands raised, if not defined in frame then it will read once. 





Required fields for "RTD" sensor type:

Required fields for "DI" sensor type:

Required fields for "DAC" sensor type:

Required fields for "IOEX" sensor type:

Required fields for "RLY" sensor type:

Required fields for "SD" sensor type:

Required fields for "SER" sensor type:

Required fields for "EROM" sensor type:

Required fields for "NORD" sensor type:

Required fields for "AO" sensor type:

Required fields for "MOT" sensor type:

Required fields for "QSPIF" sensor type:

Required fields for "USB" sensor type:

Required fields for "TDC" sensor type:

Required fields for "RAM" sensor type:

Required fields for "EMMC" sensor type:



<!-- "peripheral": String
	Available options:	[
		"GPIO", 
		"ANALOG_INPUT",
		"RTD",
		"DI",
		"DAC",
		"IOEX",
		"RELAY",
		"SD",
		"SER",
		"EROM",
		"NORD",
		"AO",
		"MOT",
		"QSPIF",
		"USB",
		"TDC",
		"RAM",
		"EMMC"] -->


