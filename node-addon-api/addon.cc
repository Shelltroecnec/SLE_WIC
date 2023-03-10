#include <napi.h>
#include <iostream>

extern "C"{
  #include "headers.h"
  #include "test.h"
  // #include "relay_cli.h"
  // #include "log.h" 
}

// extern   char Dir[];
int read_number_key_from_Obj(Napi::Env env, const char *key, Napi::Object& obj, uint32_t *data){
  if(obj.Has(Napi::String::New(env,key))){
    *data = obj.Get(Napi::String::New(env,key)).ToNumber().Uint32Value();
    return 1;
  }
  std::cout << "Undefined " << key << " key, required!!!" << std::endl;
  return 0; //Fail to read value from the object
}

// int read_string_key_from_Obj(Napi::Env env, const char *key, Napi::Object& obj, char *data){
//   if(obj.Has(Napi::String::New(env,key))){
//     *data = obj.Get(Napi::String::New(env,key)).ToString();
//     return 1;
//   }
//   std::cout << "Undefined " << key << " key, required!!!" << std::endl;
//   return 0; //Fail to read value from the object
// }

int read_double_key_from_Obj(Napi::Env env, const char *key, Napi::Object& obj, double *data){
  if(obj.Has(Napi::String::New(env,key))){
    *data = obj.Get(Napi::String::New(env,key)).ToNumber().DoubleValue();
    return 1;
  }
  std::cout << "Undefined " << key<< " key, required!!!" << std::endl;
  return 0; //Fail to read value from the object
}

Napi::Value wrap_gpio_read(const Napi::CallbackInfo& info){
  Napi::Env env = info.Env();
  Napi::Value ret;

  if (info[0].IsObject())
  {
    Napi::Object arg_obj = info[0].ToObject();
    uint32_t Port;
    uint32_t Pin;
    char dir[20];
    uint32_t Value;
    size_t len = 0;
    char val;
    // char updated_val[20];
    char *datptr = NULL;

    if(!read_number_key_from_Obj(env, "Port", arg_obj, &Port)) return ret;
    if(!read_number_key_from_Obj(env, "Pin", arg_obj, &Pin)) return ret;
    napi_get_value_string_utf8(env, arg_obj.Get(Napi::String::New(env,"dir")),dir, 100, &len);
    // if(!read_string_key_from_Obj(env, "dir", arg_obj, &dir)) return ret;
    if(!read_number_key_from_Obj(env, "Value", arg_obj, &Value)) return ret;
    printf("Parsing done for GPIO!!!!\n");
    printf("Direction: %s\n", dir);
    int gpio = IMX_GPIO_NR(Port, Pin);

    val = gpio_write_read(gpio, dir, Value); // 1/0
    
    datptr = ((val == 1) ? (char*)("High") : (char*)("Low"));

    printf("\nValue of GPIO[%d] is %s\n\n",gpio, datptr);
    // arg_obj.Set(Napi::String::New(env, "value"), Napi::Number::New(env, Value));
    // Napi::Object ret_obj = Napi::Object::New(env);
    // ret_obj.Set(Napi::String::New(env, "up_val"), Napi::Number::New(env, up_val));

    return arg_obj;
  }
  return ret;
}

Napi::Value wrap_ioexpander_cli(const Napi::CallbackInfo& info){
  Napi::Env env = info.Env();
  Napi::Value ret;

  if (info[0].IsObject())
  {
    Napi::Object arg_obj = info[0].ToObject();
    uint32_t IOEX_num;
    uint32_t Port;
    uint32_t Pin;
    char dir[20];
    uint32_t Value;
    size_t len = 0;
    char val;
    char *datptr = NULL;
    char data[20];
    if(!read_number_key_from_Obj(env, "IOEX_num", arg_obj, &IOEX_num)) return ret;
    if(!read_number_key_from_Obj(env, "Port", arg_obj, &Port)) return ret;
    if(!read_number_key_from_Obj(env, "Pin", arg_obj, &Pin)) return ret;
    napi_get_value_string_utf8(env, arg_obj.Get(Napi::String::New(env,"dir")),dir, 100, &len);
    // if(!read_string_key_from_Obj(env, "dir", arg_obj, &dir)) return ret;
    if(!read_number_key_from_Obj(env, "Value", arg_obj, &Value)) return ret;
    printf("Parsing done for IOEX!!!!\n");

    val = ioexpander_write_read(config.IOExpDev2, IOEXP_INDEX_NUMBER(IOEX_num), dir, Port, Pin, Value);
    printf("Value of Selected IOEX is %d\n\n",val);
    datptr = ((val == 1) ? (char*)("High") : (char*)("Low"));
    
    // if (val == 1){
    //     strcpy(data,"High");
    // }  else{
    //     strcpy(data, "Low");
    // } 
    printf("IOEX command completed\n\n");
    printf("Value = %s\n\n",datptr);
    // arg_obj.Set(Napi::String::New(env, "value"), Napi::Number::New(env, Value));
    // Napi::Object ret_obj = Napi::Object::New(env);
    // ret_obj.Set(Napi::String::New(env, "Min"), Napi::Number::New(env, st_wrap_ai_sample.min_samp));
    Napi::Object ret_obj = Napi::Object::New(env);
    ret_obj.Set(Napi::String::New(env, "ReadValue"), Napi::String::New(env,datptr));
    return ret_obj;
  }
  return ret;
}

Napi::Value wrap_read_analog_in(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  Napi::Value ret; //in case there is any error return this value
  st_ain_sample_t st_wrap_ai_sample;

  if(info[0].IsObject()){
    Napi::Object arg_obj = info[0].ToObject();

    memset((char *)&st_wrap_ai_sample, 0 , sizeof(st_wrap_ai_sample));
    uint32_t IO_channel;
    uint32_t sensor_type;
    uint32_t IEPE_bias_type = 0; //as AC/DC bias values are 1/2
    double IEPE_Vref = 0.0;
    uint32_t sensor_pwr_supply_src_sel;
    uint32_t sample_cnt = AIN_ADC_DEF_SAMPLES_CNT ;
    uint32_t show_raw_samples = 0;
    uint32_t scan_period = 0;
    uint32_t sampling_rate = 1000; //Minimum sampling rate
    uint32_t settling_time_ms = 100;

    //below function check if the key is part of the obj then parse it and fetch its value to the given variable
    if(!read_number_key_from_Obj(env, "channel", arg_obj, &IO_channel)) return ret;
    if(!read_number_key_from_Obj(env, "sensor_type", arg_obj, &sensor_type)) return ret;
    if(!read_number_key_from_Obj(env, "sensor_pwr_supply_sel", arg_obj, &sensor_pwr_supply_src_sel)) return ret;
    if((sensor_type == AI_IEPE_PATH) && (!read_number_key_from_Obj(env, "IEPE_bias_type", arg_obj, &IEPE_bias_type))) return ret;
    if((sensor_type == AI_IEPE_PATH) && (!read_double_key_from_Obj(env, "IEPE_Vref", arg_obj, &IEPE_Vref))) return ret;
    read_number_key_from_Obj(env, "sample_count", arg_obj, &sample_cnt);
    read_number_key_from_Obj(env, "scan_period", arg_obj, &scan_period);
    read_number_key_from_Obj(env, "sampling_rate", arg_obj, &sampling_rate);
    read_number_key_from_Obj(env, "show_raw_samples", arg_obj, &show_raw_samples);
    read_number_key_from_Obj(env, "settling_time_ms", arg_obj, &settling_time_ms);

    printf("Parsing done for Analog cli!!!!\n");
    st_wrap_ai_sample.IO_channel = (uint8_t)IO_channel;
    st_wrap_ai_sample.sensor_type = (uint8_t)sensor_type;
    st_wrap_ai_sample.IEPE_bias_type = (uint8_t)IEPE_bias_type;
    st_wrap_ai_sample.IEPE_Vref = (float)IEPE_Vref;
    st_wrap_ai_sample.ai_pwr_supply_src_sel = (uint8_t)sensor_pwr_supply_src_sel; 
    st_wrap_ai_sample.samp_cnt = sample_cnt;
    st_wrap_ai_sample.scan_period = (uint8_t)scan_period;
    st_wrap_ai_sample.sampling_rate = sampling_rate; // not yet used
    st_wrap_ai_sample.show_raw_samples = (uint8_t)show_raw_samples;
    st_wrap_ai_sample.settling_time_ms = (uint8_t)settling_time_ms;

    // printf("ch: %d, \n", st_wrap_ai_sample.IO_channel);
    // printf("type: %d\n",  st_wrap_ai_sample.sensor_type);
    // printf("pwr_select: %d\n", st_wrap_ai_sample.ai_pwr_supply_src_sel);
    // printf("IEPE_Vref: %f\n",  st_wrap_ai_sample.IEPE_Vref);
    // printf("IEPE_bias type: %d\n", st_wrap_ai_sample.IEPE_bias_type);
    // printf("samp_cnt: %d\n", st_wrap_ai_sample.samp_cnt);
    // printf("scan_period: %d\n", st_wrap_ai_sample.scan_period);
    // printf("sampling_rate: %d\n", st_wrap_ai_sample.sampling_rate);
    // printf("show_raw_samples: %d\n", st_wrap_ai_sample.show_raw_samples);
    // printf("----------------\n");

    st_ain_sample.IO_channel--; // as in current version channels are defined from 0-3 to use it as 2 bits value. but the input will be from 1-4 as physical channels.
    ain_periodic_callback(NULL, &st_wrap_ai_sample);

    // printf("Min = %d, Max = %d, Avg = %d, delta = %d \n", st_wrap_ai_sample.min_samp, st_wrap_ai_sample.max_samp, st_wrap_ai_sample.avg_cnt, st_wrap_ai_sample.max_samp - st_wrap_ai_sample.min_samp);

    Napi::Object ret_obj = Napi::Object::New(env);
    ret_obj.Set(Napi::String::New(env, "Min"), Napi::Number::New(env, st_wrap_ai_sample.min_samp));
    ret_obj.Set(Napi::String::New(env, "Max"), Napi::Number::New(env, st_wrap_ai_sample.max_samp));
    ret_obj.Set(Napi::String::New(env, "Avg"), Napi::Number::New(env, st_wrap_ai_sample.avg_cnt));
    ret_obj.Set(Napi::String::New(env, "delta"), Napi::Number::New(env, st_wrap_ai_sample.max_samp - st_wrap_ai_sample.min_samp));
    // printf("---4-------------\n");
    printf("\n");
    
    return ret_obj;
  }
  return ret;
    // printf("----5------------\n");

}

Napi::Value wrap_rtd_cli(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  Napi::Value ret; //in case there is any error return this value
  st_rtd_sample_t st_rtd_cli;
    // printf("--3--------------\n");

  if(info[0].IsObject()){
    Napi::Object arg_obj = info[0].ToObject();
    // printf("----1------------\n");

    memset((char *)&st_rtd_cli, 0 , sizeof(st_rtd_cli));
    uint32_t IO_channel;
    // uint8_t  rtd_fault:1;
    // uint32_t  counter;
    uint32_t  rtd_sample_cnt = RTD_MAX_SAMP_CNT;
    // uint32_t  channel_scan; // 1-4 for RTD channel 1 to 4 selection and 255 for all channel read
    // uint32_t scan_period = 0;
    // printf("---2-------------\n");

    //below function check if the key is part of the obj then parse it and fetch its value to the given variable
    if(!read_number_key_from_Obj(env, "channel", arg_obj, &IO_channel)) return ret;
    read_number_key_from_Obj(env, "sample_count", arg_obj, &rtd_sample_cnt);
    
    printf("Parsing for RTD done!!!!\n");
    st_rtd_cli.IO_channel = (uint8_t)IO_channel;
    st_rtd_cli.rtd_sample_cnt = (uint8_t)rtd_sample_cnt;
   

    // printf("ch: %d, \n", st_wrap_ai_sample.IO_channel);
    // printf("type: %d\n",  st_wrap_ai_sample.sensor_type);
    // printf("pwr_select: %d\n", st_wrap_ai_sample.ai_pwr_supply_src_sel);
    // printf("IEPE_Vref: %f\n",  st_wrap_ai_sample.IEPE_Vref);
    // printf("IEPE_bias type: %d\n", st_wrap_ai_sample.IEPE_bias_type);
    // printf("samp_cnt: %d\n", st_wrap_ai_sample.samp_cnt);
    // printf("scan_period: %d\n", st_wrap_ai_sample.scan_period);
    // printf("sampling_rate: %d\n", st_wrap_ai_sample.sampling_rate);
    // printf("show_raw_samples: %d\n", st_wrap_ai_sample.show_raw_samples);
    // printf("----------------\n");

    st_rtd_cli.IO_channel--; // as in current version channels are defined from 0-3 to use it as 2 bits value. but the input will be from 1-4 as physical channels.
    rtd_periodic_callback(NULL, &st_rtd_cli);

    // printf("Min = %d, Max = %d, Avg = %d, delta = %d \n", st_wrap_ai_sample.min_samp, st_wrap_ai_sample.max_samp, st_wrap_ai_sample.avg_cnt, st_wrap_ai_sample.max_samp - st_wrap_ai_sample.min_samp);

    Napi::Object ret_obj = Napi::Object::New(env);
    ret_obj.Set(Napi::String::New(env, "rtd_min_adc"), Napi::Number::New(env, st_rtd_cli.min_adc_count[0]));
    ret_obj.Set(Napi::String::New(env, "rtd_max_adc"), Napi::Number::New(env, st_rtd_cli.max_adc_count[0]));
    ret_obj.Set(Napi::String::New(env, "rtd_avg_adc"), Napi::Number::New(env, st_rtd_cli.avg_adc_count[0]));
    ret_obj.Set(Napi::String::New(env, "rtd_delta_adc"), Napi::Number::New(env, st_rtd_cli.max_adc_count - st_rtd_cli.min_adc_count));
    ret_obj.Set(Napi::String::New(env, "rtd_avg_temp"), Napi::Number::New(env, st_rtd_cli.rtd_avg_temp[0]));
    ret_obj.Set(Napi::String::New(env, "rtd_min_temp"), Napi::Number::New(env, st_rtd_cli.rtd_min_temp[0]));
    ret_obj.Set(Napi::String::New(env, "rtd_max_temp"), Napi::Number::New(env, st_rtd_cli.rtd_max_temp[0]));
    ret_obj.Set(Napi::String::New(env, "rtd_delta_temp"), Napi::Number::New(env, st_rtd_cli.rtd_max_temp - st_rtd_cli.rtd_min_temp));
    printf("\n");

    return ret_obj;
  }
  return ret;
}

Napi::Value wrap_motor_cli(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  Napi::Value ret; //in case there is any error return this value
  // st_rtd_sample_t st_rtd_cli;
    // printf("--3--------------\n");

  if(info[0].IsObject()){
    Napi::Object arg_obj = info[0].ToObject();
   
    uint32_t mot_sel;
    uint32_t dir;

    if(!read_number_key_from_Obj(env, "mot_sel", arg_obj, &mot_sel)) return ret;
    if(!read_number_key_from_Obj(env, "direction", arg_obj, &dir))return ret;
    
    printf("Parsing for Motor cli done!!!!\n");

    motor_rotation_change(mot_sel, dir);
    char Dir[20];
    if (dir == CLOCKWISE_DIR){
        strcpy(Dir,"Clockwise");
    }  else if (dir == COUNTER_CLOCKWISE_DIR){
        strcpy(Dir,"Counter-Clockwise");
    }   else if (dir == STOP_MOTOR_ROTATION){
        strcpy(Dir,"Stop Rotation");
    }
    printf("selected motor [%d] in %s direction\n\n", mot_sel, Dir);
    Napi::Object ret_obj = Napi::Object::New(env);
    ret_obj.Set(Napi::String::New(env, "MotorState"), Napi::String::New(env,Dir));
    return ret_obj;
  }
  return ret;
}

Napi::Value wrap_modbus_cli(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  Napi::Value ret; //in case there is any error return this value
  st_mod_sample_t st_mod_cli;
  // napi_status status;
  if(info[0].IsObject()){
    Napi::Object arg_obj = info[0].ToObject();
    memset((char *)&st_mod_cli, 0 , sizeof(st_mod_cli));
    uint32_t Mod_channel;
    // uint32_t baudrate;
    
    size_t len = 0;
    uint32_t selectpin;
    // char *RX_data = NULL;
    char tx_data[300];
    if(!read_number_key_from_Obj(env, "Mod_channel", arg_obj, &Mod_channel)) return ret;
    if(!read_number_key_from_Obj(env, "baudrate", arg_obj, &st_mod_cli.baudrate)) return ret;
    if(!read_number_key_from_Obj(env, "selectpin", arg_obj, &selectpin)) return ret;
    napi_get_value_string_utf8(env, arg_obj.Get(Napi::String::New(env,"TX_data")),st_mod_cli.TX_data, 100, &len);
    if(selectpin == 1){
      config.modbus_workaround_flag = selectpin;
    }
    st_mod_cli.Mod_channel = (uint8_t)Mod_channel;
    if(st_mod_cli.baudrate < 110){ //Lowest allowed baudrate 
      st_mod_cli.baudrate = 9600;
    }
    len = convert_byte_array((const char *)tx_data, (unsigned char *)st_mod_cli.TX_data, sizeof(st_mod_cli.TX_data));
    
    // st_mod_cli.TX_data = (uint8_t)TX_data;
    printf("Parsing for Modbus done!!!!\n");
    // printf("ch: %d, \n", st_wrap_ai_sample.IO_channel);
    // printf("type: %d\n",  st_wrap_ai_sample.sensor_type);
    // printf("pwr_select: %d\n", st_wrap_ai_sample.ai_pwr_supply_src_sel);
    // printf("IEPE_Vref: %f\n",  st_wrap_ai_sample.IEPE_Vref);
    // printf("IEPE_bias type: %d\n", st_wrap_ai_sample.IEPE_bias_type);
    // printf("samp_cnt: %d\n", st_wrap_ai_sample.samp_cnt);
    // printf("scan_period: %d\n", st_wrap_ai_sample.scan_period);
    // pri(ntf("sampling_rate: %d\n", st_wrap_ai_sample.sampling_rate);
    // printf("show_raw_samples: %d\n", st_wrap_ai_sample.show_raw_samples);
    // printf("----------------\n");

    // st_rtd_cli.IO_channel--; // as in current version channels are defined from 0-3 to use it as 2 bits value. but the input will be from 1-4 as physical channels.
    
    // uint32_t length;
    // status = napi_get_array_length(env, st_mod_cli.TX_data, &length);
    read_modbus(&st_mod_cli);
    // Napi::Object ret_obj = Napi::Object::New(env);
    // Napi::Object response = Napi::Object::New(env);
    std::cout << st_mod_cli.rx_len << std::endl;
    napi_value rx_array;
    napi_create_array(env, &rx_array);
    for (uint32_t i = 0; i < st_mod_cli.rx_len; ++i) {
      napi_value result;
      napi_create_int32(env, (int32_t)st_mod_cli.RX_data[i], &result);
      // int32_t test;
      // napi_get_value_int32(env, result, &test);
      // std::cout << i << " " << test << std::endl;
      napi_set_element(env, rx_array, i, result);
    }

    print_byte_array((uint8_t *)st_mod_cli.RX_data, st_mod_cli.rx_len);
    // CLOG("\nModbus received[%d]: ", ret);
    // st_mod_sample.RX_data[ret] = '\0';
    // CLOG("\nRX_Data: %s\n", st_mod_sample.RX_data);
    // print_byte_array(st_mod_sample.RX_data,ret); 
    // config.modbus_workaround_flag = temp_select_pin;
    // printf("Min = %d, Max = %d, Avg = %d, delta = %d \n", st_wrap_ai_sample.min_samp, st_wrap_ai_sample.max_samp, st_wrap_ai_sample.avg_cnt, st_wrap_ai_sample.max_samp - st_wrap_ai_sample.min_samp);

    Napi::Object ret_obj = Napi::Object::New(env);
    // ret_obj.Set(Napi::String::New(env, "ModbusResponse"), Napi::String::New(env,st_mod_cli.RX_data));
    ret_obj.Set(Napi::String::New(env, "ModbusResponse"), rx_array);
    return ret_obj;
  }
  return ret;
}

Napi::Value wrap_read_digital_in(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  Napi::Value ret; //in case there is any error return this value

  if(info[0].IsObject()){
    Napi::Object arg_obj = info[0].ToObject();
    uint32_t ch;


    //below function check if the key is part of the obj then parse it and fetch its value to the given variable
    if(!read_number_key_from_Obj(env, "channel", arg_obj, &ch)) return ret;
    
    printf("Parsing for DI done!!!!\n");


    din_periodic_callback(NULL, &ch);
    printf("\n");
    Napi::Object ret_obj = Napi::Object::New(env);
    return ret_obj;
  }
  return ret;
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  system_init();
  // exports.Set(Napi::String::New(env, "relay_operation"), Napi::Function::New(env, wrap_relay_operation));
  // std::cout << "This is init call................" << std::endl;
  exports.Set(Napi::String::New(env, "analog_in_read"), Napi::Function::New(env, wrap_read_analog_in));
  exports.Set(Napi::String::New(env, "rtd_cli"), Napi::Function::New(env, wrap_rtd_cli));
  exports.Set(Napi::String::New(env, "digital_in_read"), Napi::Function::New(env, wrap_read_digital_in));
  exports.Set(Napi::String::New(env, "motor_cli"), Napi::Function::New(env, wrap_motor_cli));
  exports.Set(Napi::String::New(env, "gpio_read"), Napi::Function::New(env, wrap_gpio_read));
  exports.Set(Napi::String::New(env, "modbus_read"), Napi::Function::New(env, wrap_modbus_cli));
  exports.Set(Napi::String::New(env, "ioex_cli"), Napi::Function::New(env, wrap_ioexpander_cli));
  
  // system_Deinit();
  
  return exports;

}


NODE_API_MODULE(addon, Init)
