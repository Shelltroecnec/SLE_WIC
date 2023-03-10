module.exports = function(RED) {
    var addon = require('bindings')('ultrafab_ll_addon');
    function ioexNode(config) {
        RED.nodes.createNode(this,config);

        // Call all inputs from user here, config.'*' is where * represents the name in form
        this.ioExpanderSelector = parseInt(config.ioExpanderSelector);
        this.port = parseInt(config.port);
        this.pin = parseInt(config.pin);
        this.direction = config.direction;
        this.value = parseInt(config.value);
        

        var node = this;
        // Actual Function of the Node Here
        node.on('input', function(msg) {

            msg.payload  = addon.ioex_cli({IOEX_num: node.ioExpanderSelector, Port: node.port, Pin: node.pin, dir: node.direction, Value: node.value}); 
            
            node.send(msg);
        });
    }
    //  Name of Node first and name of function second
    RED.nodes.registerType("ioex",ioexNode);
}