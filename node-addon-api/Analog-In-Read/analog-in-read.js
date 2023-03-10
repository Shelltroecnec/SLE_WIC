module.exports = function(RED) {
    var addon = require('bindings')('ultrafab_ll_addon');
    function analogInReadNode(config) {
        RED.nodes.createNode(this,config);

        // Call all inputs from user here, config.'*' is where * represents the name in form
        this.channelNo = parseInt(config.channelNo);
        this.sensorType = parseInt(config.sensorType);
        this.power = parseInt(config.power);
        this.sampleNo = parseInt(config.sampleNo);
        this.sampleFreq = parseInt(config.sampleFreq);
        // this.rawSample = parseInt(config.rawSample);
        this.settlingTime = parseInt(config.settlingTime);
        this.bias = parseInt(config.bias);
        this.vref = Number(config.vref);
        

        var node = this;
        // Actual Function of the Node Here
        node.on('input', function(msg) {
            if ((this.sampleNo < 0) || (this.sampleNo > 10000))
            { 
                this.status({fill:"red",shape:"ring",text:"No. of Samples: Error!"});
                msg.payload = {payload: "Invalid No. Of Samples"};
            }
            else if ((this.sampleFreq < 0) || (this.sampleFreq > 1000))
            { 
                this.status({fill:"red",shape:"ring",text:"Sample Frequency: Error!"});
                msg.payload = {payload: "Invalid Sampling Frequency"};
            }
            else if ((this.settlingTime < 0))
            { 
                this.status({fill:"red",shape:"ring",text:"Settling Time: Error!"});
                msg.payload = {payload: "Invalid Settling Time"};
            }
            else
            {
                msg.payload = addon.analog_in_read({channel: node.channelNo, sensor_type: node.sensorType, sensor_pwr_supply_sel: node.power, IEPE_bias_type: node.bias, IEPE_Vref: node.vref, sampling_rate: node.sampleFreq, show_raw_samples: 0, settling_time_ms:node.settlingTime, sample_count: node.sampleNo, scan_period: 10});  
            }
            node.send(msg);
        });
    }
    //  Name of Node first and name of function second
    RED.nodes.registerType("analog-in-read",analogInReadNode);
}