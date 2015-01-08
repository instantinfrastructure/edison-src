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

/**
 * @file A basic client that sends a message 'hello' to a compatible server. It then
 * prints out the message sent back by the server.
 * @see example/basic-client-server/server.js
 */
var iotkit = require('iotkit-comm');
var path = require('path');

var query = new iotkit.ServiceQuery(path.join(__dirname, "server-query.json"));
iotkit.createClient(query, function (client) {
  client.comm.setReceivedMessageHandler(function (message, context) {
    console.log("received from server: " + message.toString());
    client.comm.send("hello");
  });
  client.comm.send("hello");
});
