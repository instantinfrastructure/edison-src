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

/** @module client */

var PluginManager = require("./plugin-manager.js");

/** Communication plugin used to interact with the service */
Client.prototype.comm = null;

/** Specification of the service this client will be connecting to */
Client.prototype.spec = null;

/** Directory service to find services */
Client.prototype.directory = null;

/**
 * Initializes a client object that can connect to a service
 * described by serviceSpec.
 * @param {object} serviceSpec - {@tutorial service-spec-query}
 * @constructor module:client~Client
 */
function Client(serviceSpec, serviceDirectory) {
  "use strict";

  this.spec = serviceSpec;
  this.directory = serviceDirectory;
  this.comm = null;

  var commplugin;
  try {
    commplugin =  PluginManager.getClientPlugin(serviceSpec.type.name);
  } catch (err) {
    console.log("ERROR: Could not load communication plugin needed to interact with service at '" +
      serviceSpec.address + ":" + serviceSpec.port + "'. Plugin '" + serviceSpec.type.name + "' was not found or produced errors while loading.");
    console.log(err);
    return;
  }

  this.comm = new commplugin(serviceSpec);
}

module.exports = Client;