module.exports = function(RED) {
    var addon = require('bindings')('ultrafab_ll_addon');
    function modbusNode(config) {
        RED.nodes.createNode(this,config);

        // Call all inputs from user here, config.'*' is where * represents the name in form
        this.channelNo = parseInt(config.channelNo);
        this.baudrate = parseInt(config.baudrate);
        this.modbusFrame = config.modbusFrame;
        

        var node = this;
        // Actual Function of the Node Here
        node.on('input', function(msg) {
            if ((this.baudrate < 0))
            { 
                this.status({fill:"red",shape:"ring",text:"Baudrate: Error!"});
                msg.payload = {payload: "Invalid Baudrate"};
            }
            else
            {       
                msg.payload = addon.modbus_read({Mod_channel: node.channelNo, baudrate: node.baudrate, TX_data: node.modbusFrame, selectpin: 1});  
            }
            node.send(msg);
        });
    }
    //  Name of Node first and name of function second
    RED.nodes.registerType("modbus",modbusNode);
}