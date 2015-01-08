/*
 * Copyright (c) 2014, Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU Lesser General Public License,
 * version 2.1, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
 * more details.
 *
 * Created by Pradeep.
 */

var mqtt = require('mqtt');

/**
 * Name of interface this plugin would like to adhere to ({@tutorial plugin})
 * @type {string}
 */
EnableIoTCloudClient.prototype.interface = "enableiot-cloud-client-interface";

/**
 * The internal client object this plugin ({@tutorial plugin}) creates to communicate with the {@tutorial cloud}.
 * @type {object}
 */
EnableIoTCloudClient.prototype.client = {};

/**
 * The client-side plugin needs a received message handler to process messages coming from the {@tutorial cloud}
 * ({@tutorial plugin}).
 * @type {function}
 */
EnableIoTCloudClient.prototype.receivedMsgHandler = null;

/**
 * Topic to publish or subscribe data to. This is the only topic
 * the cloud supports.
 */
EnableIoTCloudClient.prototype.topic = "data";


/**
 * Create a client that connects to the {@tutorial cloud}. The cloud is described just like any other service:
 * by using a service specification.
 * @param serviceSpec {object} specification for the cloud ({@tutorial service-spec-query})
 * @constructor
 */
function EnableIoTCloudClient(serviceSpec) {
  this.client = mqtt.createClient(serviceSpec.port, serviceSpec.address);

  var self = this;
  this.client.on('message', function (topic, message) {
    if (self.receivedMsgHandler) {
      self.receivedMsgHandler(message, {event: 'message', topic: topic});
    }
  });
}

/**
 * Register a new sensor with the cloud. Once registered, data can be published to the
 * cloud for this sensor (see {@tutorial cloud}).
 * @param sensorname {string} name of the sensor to register
 * @param type {string} supported types are 'temperature.v1.0', 'humidity.v1.0' ({@tutorial cloud})
 * @param unit {string} not supported yet. This parameter is ignored.
 */
EnableIoTCloudClient.prototype.registerSensor = function(sensorname, type, unit){
    this.client.publish("data", JSON.stringify({"n":sensorname, "t": type}));
}

/**
 * Send a message to the cloud broker. Equivalent to publishing to a topic.
 * @param msg {string} Message to send to cloud broker (see {@tutorial cloud}).
 * @param context {object.<string, string>} context.topic is contains the topic string
 */
EnableIoTCloudClient.prototype.publish = function (msg, context) {
    this.client.publish(EnableIoTCloudClient.prototype.topic, msg);
};

///**
// * subscribe to data published by all your sensors. No other topic subscriptions are supported
// */
//EnableIoTCloudClient.prototype.subscribe = function () {
//  this.client.subscribe(EnableIoTCloudClient.prototype.topic);
//};

/**
 * Not supported at the moment.
 * @param topic
 */
EnableIoTCloudClient.prototype.unsubscribe = function (topic) {
    "use strict";
};

/**
 * Set a handler for all received messages.
 * @param handler {function} called when a message is received
 */
EnableIoTCloudClient.prototype.setReceivedMessageHandler = function(handler) {
    "use strict";
    this.receivedMsgHandler = handler;
};

/**
 * close connection. Sends FIN to the {@tutorial cloud}.
 */
EnableIoTCloudClient.prototype.done = function () {
    this.client.end();
};

module.exports = EnableIoTCloudClient;
