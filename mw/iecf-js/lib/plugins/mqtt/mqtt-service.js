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
 * Created by adua.
 */

var mqtt = require('mqtt');

/**
 * Name of interface this plugin would like to adhere to ({@tutorial plugin})
 * @type {string}
 */
MQTTService.prototype.interface = "service-interface";

/**
 * The service instance this plugin creates ({@tutorial Service}).
 * @type {object}
 */
MQTTService.prototype.service = {};

/**
 * The server-side plugin ({@tutorial plugin}) needs a received message handler to process
 * messages coming from the clients.
 * @type {function}
 */
MQTTService.prototype.receivedMsgHandler = null;

/**
 * Create a minimal MQTT broker based on the given specification ({@tutorial Service}).
 * @param serviceSpec {object} {@tutorial service-spec-query}
 * @constructor
 */
function MQTTService(serviceSpec) {
  "use strict";

  var self = this;
  this.service = mqtt.createServer(function(client) {
    client.on('connect', function(packet) {
      if (self.receivedMsgHandler) {
        self.receivedMsgHandler(client, packet, {event: 'connect'});
      }
    });

    client.on('publish', function(packet) {
      if (self.receivedMsgHandler) {
        self.receivedMsgHandler(client, packet, {event: 'publish'});
      }
    });

    client.on('subscribe', function(packet) {
      if (self.receivedMsgHandler) {
        self.receivedMsgHandler(client, packet, {event: 'subscribe'});
      }
    });

    client.on('pingreq', function(packet) {
      if (self.receivedMsgHandler) {
        self.receivedMsgHandler(client, packet, {event: 'pingreq'});
      }
    });

    client.on('disconnect', function(packet) {
      if (self.receivedMsgHandler) {
        self.receivedMsgHandler(client, packet, {event: 'disconnect'});
      }
    });

    client.on('close', function(err) {
      if (self.receivedMsgHandler) {
        self.receivedMsgHandler(client, err, {event: 'close'});
      }
    });

    client.on('error', function(err) {
      if (self.receivedMsgHandler) {
        self.receivedMsgHandler(client, err, {event: 'error'});
      }
    });
  });

  this.service.listen(serviceSpec.port);
}

/**
 * Send a message to a client.
 * @param client {object} instance of client on the server-side
 * @param msg {string} message to send
 * @param context {object.<string, string>} describes what to do with message.
 * <ul>
 * <li> If empty, message is sent as-is to the client </li>
 * <li> If context.ack is set, then an acknowledgement to the given message is sent
 * as described by the MQTT protocol (also see {@link sample-apps/mqtt-mini-broadcast-broker.js}).
 * <ul>
 * <li> If context.ack is 'connack', then a connection acknowledgement is returned</li>
 * <li> if it is 'suback', then a subscription acknowledgement is returned</li>
 * <li> if it is 'pingrep', then response to the ping request is sent</li>
 * <li> otherwise, simply send message to client</li></ul></li></ul>
 */
MQTTService.prototype.sendTo = function (client, msg, context) {

  if (!context) {
    client.publish(msg);
    return;
  }

  switch (context.ack) {
    case 'connack':
      client.connack({returnCode: 0});
      break;
    case 'suback':
      var granted = [];
      for (var i = 0; i < msg.subscriptions.length; i++) {
        granted.push(msg.subscriptions[i].qos);
      }
      client.suback({granted: granted, messageId: msg.messageId});
      break;
    case 'pingresp':
      client.pingresp();
      break;
    default:
      client.publish(msg);
  }
};

/**
 * Does nothing for now. Eventually might be used to broadcast messages to all clients, or
 * publish status messages to another portal etc.
 * @param msg {string} message to publish
 * @param context {object} the plugin looks at properties in this object to determine if it should
 * process the given message in any special way.
 */
MQTTService.prototype.publish = function (msg, context) {
  "use strict";

};

/**
 * Set a handler for all received messages
 * @param handler {function}
 */
MQTTService.prototype.setReceivedMessageHandler = function(handler) {
  "use strict";
  this.receivedMsgHandler = handler;
};

/**
 * End the server. Currently does nothing. Could possibly handle signals here.
 */
MQTTService.prototype.done = function () {
};

/**
 * Perform actions related to client instances on server side. For example, if
 * an error occurs on the communication stream, destroy the stream.
 * @param client {object} client instance on server-side
 * @param context {object} properties here indicate what action to take on the client instance.
 * If context.action is 'endstream', close the connection stream between server and client.
 */
MQTTService.prototype.manageClient = function (client, context) {
  "use strict";
  switch (context.action) {
    case 'endstream':
      client.stream.end();
  }
};

module.exports = MQTTService;