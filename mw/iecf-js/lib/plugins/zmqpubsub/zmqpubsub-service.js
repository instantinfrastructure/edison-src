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
 */

var zeromq = require('zmq');

ZMQPubSubService.prototype.interface = "service-interface";
ZMQPubSubService.prototype.socket = null;
ZMQPubSubService.prototype.receivedMsgHandler = null;

/**
 * Create a zmq publisher based on the given service specification
 * @param serviceSpec {object} {@tutorial service-spec-query}
 * @see {@link http://zeromq.org}
 * @constructor
 */
function ZMQPubSubService(serviceSpec) {
    "use strict";
  this.socket = zeromq.socket('pub');
  if (serviceSpec.address) {
    this.socket.bindSync('tcp://' + serviceSpec.address + ':' + serviceSpec.port);
  } else {
    this.socket.bindSync('tcp://*:' + serviceSpec.port);
  }
}

ZMQPubSubService.prototype.sendTo = function (client, msg, context) {

};

ZMQPubSubService.prototype.publish = function (msg, context) {
  "use strict";
  this.socket.send(msg);
};

ZMQPubSubService.prototype.manageClient = function (client, context) {

};

ZMQPubSubService.prototype.setReceivedMessageHandler = function (handler) {

};

ZMQPubSubService.prototype.done = function () {
    this.socket.close();
};

module.exports = ZMQPubSubService;