
module.exports = function(RED) {
  var addon = require('bindings')('ultrafab_ll_addon');
    function rtdNode(config) {
        RED.nodes.createNode(this,config);

        // Call all inputs from user here, config.'*' is where * represents the name in form
        this.channelNo = parseInt(config.channelNo);
        this.sampleCount = config.sampleCount;
        var node = this;
        var err = "Invalid Sample Count";
        // Actual Function of the Node Here
        node.on('input', function(msg) {
           if ((this.sampleCount < 0) || (this.sampleCount > 100))
           {
             msg = {payload: "Invalid Sample Count"};
           }
           else
           {
            msg.payload = addon.rtd_cli({channel: node.channelNo, sample_count: node.sampleCount});

           }
           node.send(msg);
        });
    }
    //  Name of Node first and name of function second
    RED.nodes.registerType("rtd",rtdNode);
}