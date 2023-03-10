var addon = require('bindings')('ultrafab_ll_addon')

// console.log('This should be nine:', addon.add(6,5))

// addon.relay_operation(1,1,0);
// addon.read_analog_in(1);
var channel_num = parseInt(process.argv[2])
// var sensor_type = parseInt(process.argv[3])
// var sensor_pwr_supply_sel = parseInt(process.argv[4])
// var sample_count = parseInt(process.argv[5])
// var sampling_rate = parseInt(process.argv[6])
// var show_raw_samples = parseInt(process.argv[7])
// var settling_time_ms = parseInt(process.argv[8])
// var IEPE_bias_type = parseInt(process.argv[9])
// var IEPE_Vref = parseInt(process.argv[10])
// if (process.argv[2] == undefined || channel_num > 4 || channel_num < 1){
// 	channel_num = 1;
// } 
// console.log("Selected channel: " + channel_num);
// console.log("sensor_type: " + sensor_type);
// console.log("csensor_pwr_supply_sel : " + sensor_pwr_supply_sel);
// console.log("sample_count: " + sample_count);
// console.log("sampling_rate: " + sampling_rate);
// console.log("show_raw_samples: " + show_raw_samples);
// console.log("settling_time_ms: " + settling_time_ms);
// console.log("IEPE_bias_type: " + IEPE_bias_type);
// console.log("IEPE_Vref: " + IEPE_Vref);

// ret = addon.analog_in_read({channel: channel_num, sensor_type: sensor_type, sensor_pwr_supply_sel: sensor_pwr_supply_sel, IEPE_bias_type: IEPE_bias_type, IEPE_Vref:IEPE_Vref, sampling_rate: sampling_rate, show_raw_samples:show_raw_samples, settling_time_ms:settling_time_ms, sample_count:sample_count, scan_period: 10});
// ret1 = addon.rtd_cli({channel: channel_num, sample_count:10});
// ret2 = addon.digital_in_read({channel: channel_num});
// ret3 = addon.motor_cli({mot_sel : 1, direction: 2});
// ret4 = addon.gpio_read({Port: 3, Pin: 9, dir: "in", Value: 1});
// ret5 = addon.modbus_read({Mod_channel: 1, baudrate: 9600, TX_data: "010400000002", selectpin: 1});
ret6 = addon.ioex_cli({IOEX_num: 1, Port: 1, Pin: 1, dir: "read", Value: 0});
console.log(ret6);
console.log("In NodeJS AI output:")
// console.log(ret)