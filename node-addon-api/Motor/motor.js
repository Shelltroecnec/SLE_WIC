module.exports = function(RED) {
    var addon = require('bindings')('ultrafab_ll_addon');
    function motorNode(config) {
        RED.nodes.createNode(this,config);

        // Call all inputs from user here, config.'*' is where * represents the name in form
        this.motorNo = parseInt(config.motorNo);
        this.direction = parseInt(config.direction);

        var node = this;
        // Actual Function of the Node Here
        node.on('input', function(msg) {
            msg.payload = addon.motor_cli({mot_sel: node.motorNo , direction: node.direction});

            node.send(msg);
        });
    }
    //  Name of Node first and name of function second
    RED.nodes.registerType("motor",motorNode);
}