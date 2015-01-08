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

ZMQReqRepService.prototype.interface = "service-interface";
ZMQReqRepService.prototype.socket = null;
ZMQReqRepService.prototype.receivedMsgHandler = null;

/**
 * Create a zmq replier based on the given service specification
 * @param serviceSpec {object} {@tutorial service-spec-query}
 * @see {@link http://zeromq.org}
 * @constructor
 */
function ZMQReqRepService(serviceSpec) {
  "use strict";
  this.socket = zeromq.socket('rep');
  if (serviceSpec.address) {
    this.socket.bindSync('tcp://' + serviceSpec.address + ':' + serviceSpec.port);
  } else {
    this.socket.bindSync('tcp://*:' + serviceSpec.port);
  }

  var self = this;
  this.socket.on('message', function (message) {
    if (self.receivedMsgHandler) {
      self.receivedMsgHandler(self.socket, message, {event: 'message'});
    }
  });
}

ZMQReqRepService.prototype.sendTo = function (client, msg, context) {
  "use strict";
  this.socket.send(msg);
};

ZMQReqRepService.prototype.publish = function (msg, context) {
  "use strict";
};

ZMQReqRepService.prototype.manageClient = function (client, context) {
};

ZMQReqRepService.prototype.setReceivedMessageHandler = function (handler) {
  "use strict";
  this.receivedMsgHandler = handler;
};

ZMQReqRepService.prototype.done = function () {
  this.socket.close();
};

module.exports = ZMQReqRepService;