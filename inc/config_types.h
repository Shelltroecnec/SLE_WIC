/**
 * @file config_types.h
 * @author Fakhruddin Khan (f.khan@acevin.com)
 * @brief 
 * @version 0.1
 * @date 2021-08-11
 * 
 * @copyright Copyright (c) 2021
 * 
 */



INT(verbose, 0);
INT(quiet, 0);
INT(maxConnFailCnt_reboot, 40);
INT(log_level, 2); //log levels, 0 to 5, refer the enum log_levels for more detail, by default CLI, ERROR and WARNING are enabled
INT(cli_rtd_op_prd, 2);
INT(cli_din_op_prd, 5);

STRING(logFile, "/home/root/ultrafab/dameon.log", 256);

/* IO EXpanders -WIC */
STRING(IOExpDev, "/dev/i2c-5", 32); //<- To Do : Validate i2c devices
STRING(IOExpDev2, "/dev/i2c-2", 32); //<- To Do : Validate i2c devices
STRING(IOExpDev3, "/dev/i2c-3", 32); //<- To Do : Validate i2c devices


STRING(LVDSDev, "/dev/i2c-2", 32);
STRING(eeprom_dev, "/dev/i2c-1", 32);
STRING(qspi_flash_dev, "/dev/mtd0", 32);
STRING(rtc_dev, "/dev/i2c-5", 32); //<-Todo Validate

//--------------- MQTT related configuration ----------------------------------------------------
STRING(gateway_id, "",32);
INT(mqttTraceLevel, 0);

/* MQTT options */
INT(MQTTVersion, MQTTVERSION_DEFAULT);
STRING(to_logger_topic_prefix, "ultrafab/flow", 32);
STRING(to_server_topic_prefix, "ultrafab/ulad", 32);
STRING(clientid, "mqtt_ultrafab", 50);
INT(qos, 0);
INT(retained, 0);

/* Mqtt config */
STRING(username, "mqtt", 50);
STRING(password, "mqtt", 50);
STRING(host, "43.225.161.182", 50);
STRING(port, "1883", 50);

// STRING(mqtt_broker_url, "tcp://35.225.129.135:1883", 50); //Itanta broker
STRING(mqtt_broker_url, "43.225.161.182:1883", 64); //Acevin broker
INT(MqttKeepalive, 30);
INT(sensor_settling_time_ms, 500);


//USB validation
INT(bus1devices, 5);
INT(bus2devices, 4);

//LVDS validation 
STRING(demoimg, "demo_img.png", 50);
	
/* TLS options */
INT(insecure, 0);
STRING(capath, "", 50);
STRING(cert, "", 50);
STRING(cafile, "", 50);
STRING(key, "", 50);
STRING(keypass, "", 50);
STRING(ciphers, "", 50);
STRING(psk_identity, "", 50);
STRING(psk, "", 50);

//------------ end of MQTT config
STRING(rtd_spi_device, "/dev/spidev0.0", 32);
STRING(ad7988_dev, "/dev/spidev1.0", 32);
//STRING(ads7038_dev, "/dev/spidev7.2", 32);
// STRING(dac1_dev, "/dev/spidev0.1", 32);
// STRING(dac2_dev, "/dev/spidev0.2", 32);
STRING(mcp48f_dac_dev, "/dev/spidev6.0", 32);
STRING(nordic_spi_device, "/dev/spidev1.2", 32);
STRING(W25Q32_spi_device, "/dev/spidev8.0", 32);

//STRING(ltc2634_spi_device, "/dev/spidev1.2", 32);

STRING(sd_card_test_file, "test.sd_card", 64);
STRING(sd_card_mount_path, "/mnt/sdcard", 64);

STRING(usb1_mount_path, "mount /dev/sda /mnt/usb1", 64);
STRING(usb2_mount_path, "mount /dev/sdb /mnt/usb2", 64);

STRING(emmc_flash_test_file, "/mnt/emmc/test.emmc_flash", 64);

STRING(ser_rs485_232_dev, "/dev/ttymxc2", 32);

STRING(ser_simcard_dev, "/dev/ttymxcX", 32); //TODO: validate serial port

INT(debugLoglevel, 3); // 1 - err, 2 - warn, 3 - info, 4 - debug

//IO configuration for Analog section: IOE1-P-_2 -- IO expander 1 - port 1 - pin 2
STRING(pwr_supply_sel_pin, "IOE1-1-2", 20);

//below GPIO are used to select the sensor type muxing for AD7988 IC
STRING(analog_path_sel_a0_pin, "GPIO-39", 20);
STRING(analog_path_sel_a1_pin, "GPIO-40", 20);
STRING(analog_path_sel_a2_pin, "GPIO-26", 20);

//below GPIO are for selecting the channels of Sensor connected on header, active high signal
STRING(analog_ch_sel_s0_pin, "GPIO-64", 20);
STRING(analog_ch_sel_s1_pin, "GPIO-87", 20);
STRING(analog_ch_sel_s2_pin, "GPIO-28", 20);

//Mux enable pin for path and channel select of the sensors, MUX_EN_SENSOR O/P
STRING(analog_path_ch_sel_mux_en_pin, "IOE1-1-1", 20);

//to select the pwr supply for different sensor, POWER SELECT
STRING(ops_10_24V_pwr_sel_pin, "IOE0-1-2", 20); // Output power supply select pin - 0: 24V line select, 1: 10V Power line select

//Mux enable pin for pwr selection for sensors, MUX_EN_PWR
STRING(analog_pwr_sel_mux_en_pin, "IOE1-0-2", 20);

//SLE-Smartio SC606 analog power select mux
// STRING(analog_pwr_sel_s0_pin, "IOE0-39", 20);
STRING(analog_pwr_sel_pin, "IOE1-0-3", 20);

//4-20 mAmps pwr enable control pin (420_PWR_CTL)
STRING(ops_420m_pwr_ctrl_pin, "IOE1-0-1", 20); //output power supply 420mAmp power control pin

//Enable pwr for vibration sensor (IEPE_CTL pin)
STRING(IEPE_ctl_pin, "IOE0-1-3", 20);

//Enable 12v power supply
STRING(analog_12v_pwr_en_pin, "IOE1-0-7", 20);

//Vibration sensor Bias selection GAIN_CTL_IEPE
STRING(IEPE_gain_ctl_pin, "GPIO-4-31", 20);

//RTD Mux enable pin definition
// STRING(rtd_mux_en_pin, "GPIO-5-4", 20);

//SLE RTD Mux enable pin definition
STRING(rtd_mux_en_pin, "IOE1-0-0", 20);

//RTD channel select pins
// STRING(rtd_ch_sel_a0_pin, "GPIO-3-25", 20);
// STRING(rtd_ch_sel_a1_pin, "GPIO-3-19", 20);

//SLE RTD channel select pins
STRING(rtd_ch_sel_a0_pin, "GPIO-8", 20);
STRING(rtd_ch_sel_a1_pin, "GPIO-27", 20);

//Digital inputs
// STRING(digital_in_1_pin, "GPIO-1-8", 20);
// STRING(digital_in_2_pin, "GPIO-1-9", 20);
// STRING(digital_in_3_pin, "GPIO-1-10", 20);
// STRING(digital_in_4_pin, "GPIO-1-11", 20);
// STRING(digital_in_5_pin, "GPIO-1-12", 20);
// STRING(digital_in_6_pin, "GPIO-1-13", 20);
// STRING(digital_in_7_pin, "GPIO-1-14", 20);
// STRING(digital_in_8_pin, "GPIO-1-15", 20);

//Digital inputs
// STRING(digital_in_1_pin, "GPIO-44", 20);
// STRING(digital_in_2_pin, "GPIO-129", 20);
// STRING(digital_in_3_pin, "GPIO-43", 20);
// STRING(digital_in_4_pin, "GPIO-9", 20);
// STRING(digital_in_5_pin, "GPIO-65", 20);
// STRING(digital_in_6_pin, "GPIO-45", 20);
// STRING(digital_in_7_pin, "GPIO-139", 20);
// STRING(digital_in_8_pin, "GPIO-133", 20);

//WIC - Digital inputs
STRING(digital_in_1_pin, "IOE2-0-0", 20);
STRING(digital_in_2_pin, "IOE2-0-1", 20);
STRING(digital_in_3_pin, "IOE2-0-2", 20);
STRING(digital_in_4_pin, "IOE2-0-3", 20);
STRING(digital_in_5_pin, "IOE2-0-4", 20);
STRING(digital_in_6_pin, "IOE2-0-5", 20);
STRING(digital_in_7_pin, "IOE2-0-6", 20);
STRING(digital_in_8_pin, "IOE2-0-7", 20);
STRING(digital_in_9_pin, "IOE2-1-0", 20);
STRING(digital_in_10_pin, "IOE2-1-1", 20);
STRING(digital_in_11_pin, "IOE2-1-2", 20);
STRING(digital_in_12_pin, "IOE2-1-3", 20);
STRING(digital_in_13_pin, "IOE2-1-4", 20);
STRING(digital_in_14_pin, "IOE2-1-5", 20);
STRING(digital_in_15_pin, "IOE2-1-6", 20);
STRING(digital_in_16_pin, "IOE2-1-7", 20);


STRING(ssr_cathode_1, "IOE0-1-5", 20);
STRING(ssr_cathode_2, "IOE0-1-4", 20);
STRING(ssr_cathode_3, "IOE0-1-3", 20);
STRING(ssr_cathode_4, "IOE0-1-2", 20);
STRING(ssr_cathode_5, "IOE0-1-1", 20);
STRING(ssr_cathode_6, "IOE0-1-0", 20);
STRING(ssr_24V_dry_contact_relay, "IOE0-0-7", 20);

/* WIC - Digital Out*/
STRING(digital_out_1_pin, "IOE1-0-0", 20);
STRING(digital_out_2_pin, "IOE1-0-1", 20);
STRING(digital_out_3_pin, "IOE1-0-2", 20);
STRING(digital_out_4_pin, "IOE1-0-3", 20);
STRING(digital_out_5_pin, "IOE1-0-4", 20);
STRING(digital_out_6_pin, "IOE1-0-5", 20);
STRING(digital_out_7_pin, "IOE1-0-6", 20);
STRING(digital_out_8_pin, "IOE1-0-7", 20);
STRING(digital_out_9_pin, "IOE1-1-0", 20);
STRING(digital_out_10_pin, "IOE1-1-1", 20);
STRING(digital_out_11_pin, "IOE1-1-2", 20);
STRING(digital_out_12_pin, "IOE1-1-3", 20);
STRING(digital_out_13_pin, "IOE1-1-4", 20);
STRING(digital_out_14_pin, "IOE1-1-5", 20);
STRING(digital_out_15_pin, "IOE1-1-6", 20);
STRING(digital_out_16_pin, "IOE1-1-7", 20);

//GPIO configuration for the Serial interface
STRING(ser_rs485_232_select_pin, "GPIO-4-27", 20);
STRING(ser_rs485_232_mux_en_pin, "IOE0-1-5", 20); // this is not required as the pullup is already there so Mux is always enabled
STRING(ser_mux_S0_pin, "GPIO-5-1", 20);
STRING(ser_mux_S1_pin, "GPIO-5-0", 20);
STRING(ser_rs485_r_en_pin, "GPIO-4-24", 20);
STRING(ser_rs485_d_fast_en_pin, "GPIO-4-25", 20);

INT(cli_aout_dac_wr_prd, 1000);
INT(cli_aout_dac_bc_inc, 100);

// STRING(motor_INA1, "GPIO-1-6", 20);
// STRING(motor_INB1, "GPIO-1-7", 20);
// STRING(motor_ENA1, "GPIO-1-5", 20);
// STRING(motor_ENB1, "GPIO-1-4", 20);

//SLE motor pins
STRING(motor_EN1, "IOE0-0-0", 20);
STRING(motor_EN2, "IOE0-0-1", 20);
STRING(motor_PWMA1, "GPIO-66", 20);
STRING(motor_PWMB1, "GPIO-36", 20);
STRING(motor_PWMA2, "GPIO-33", 20);
STRING(motor_PWMB2, "GPIO-33", 20);//Check with HW team
STRING(motor_nSLEEP1, "IOE0-1-7", 20);
STRING(motor_nSLEEP2, "IOE0-1-6", 20);




// STRING(motor_INA2, "GPIO-4-22", 20);
// STRING(motor_INB2, "GPIO-4-23", 20);
// STRING(motor_ENA2, "GPIO-4-21", 20);
// STRING(motor_ENB2, "GPIO-4-26", 20);

INT(cli_motor_scan_prd, 5);
DOUBLE(mot_cli_iconst_mul, 7);

//Modbus Flag status
INT(modbus_workaround_flag, 1);
INT(modbus_delay_ms, 100);


//Analog in CLI configuration
INT(cli_ain_sample_cnt, 50);
INT(cli_ain_sample_freq, 1000);
INT(cli_ain_scan_period, 5);
INT(cli_ain_tdc_polarity, 0);
// STRING(cli_ain_tdc_polarity_pin, "GPIO-1-0", 20);

//Rtd in CLI config
// INT(cli_rtd_scan_period, 5);

//float
DOUBLE(ain_iepe_vref, 5.0);
DOUBLE(ain_tdc_vref, 2.5);


INT(mutex_lock_timeout_sec, 20); //if a process waits for this much seconds then it will unlock the process if lock is not acquire by it self


INT(pwm_frequency, 20000);
INT(ramp_period_sec, 50);
INT(pwm_duty_cycle_step, 10);

//ioec RESET
//- reset1 - gpio-2
//-reset2 - gpio -0