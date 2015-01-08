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
var zeromq = require('zmq');

ZMQPubSubClient.prototype.interface = "client-interface";
ZMQPubSubClient.prototype.socket = null;
ZMQPubSubClient.prototype.receivedMsgHandler = null;

/**
 * Create a zmq subscriber that connects to a publisher described by the given service specification
 * @param serviceSpec {object} {@tutorial service-spec-query}
 * @see {@link http://zeromq.org}
 * @constructor
 */
function ZMQPubSubClient(serviceSpec) {
  "use strict";
  this.socket = zeromq.socket('sub');
  this.socket.connect('tcp://' + serviceSpec.address + ':' + serviceSpec.port);

  var self = this;
  this.socket.on('message', function (message) {
    if (self.receivedMsgHandler) {
      self.receivedMsgHandler(message, {event: 'message'});
    }
  });
}

ZMQPubSubClient.prototype.send = function (msg, context) {
  "use strict";

};

ZMQPubSubClient.prototype.subscribe = function (topic) {
    this.socket.subscribe(topic);
};

ZMQPubSubClient.prototype.setReceivedMessageHandler = function (handler) {
  "use strict";
  this.receivedMsgHandler = handler;
};

ZMQPubSubClient.prototype.unsubscribe = function (topic) {
  "use strict";
};

ZMQPubSubClient.prototype.done = function () {
    this.socket.close();
};

module.exports = ZMQPubSubClient;