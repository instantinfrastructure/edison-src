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

/** @module service */

var PluginManager = require("./plugin-manager.js");

/** communication plugin used to interact with the service */
Service.prototype.comm = null;

/** specification of the service this client will be connecting to */
Service.prototype.spec = null;

/** the service-directory instance that advertises this service */
Service.prototype.directory = null;

/**
 * Create a service instance using the given specification
 * @param serviceSpec {object} ({@tutorial service-spec-query})
 * @constructor module:service~Service
 */
function Service(serviceSpec) {
  "use strict";

  this.spec = serviceSpec;

  var commplugin;
  try {
    commplugin =  PluginManager.getServicePlugin(this.spec.type.name);
  } catch (err) {
    console.log("ERROR: An appropriate communication plugin could not be found for service '" + this.spec.name +
      "'. Service needs communication plugin '" + this.spec.type.name + "'.");
    throw err;
  }

  this.comm = new commplugin(serviceSpec);
}

/** If this service is advertised on the LAN, call this function */
Service.prototype.setDirectory = function (serviceDirectory) {
  this.directory = serviceDirectory;
}

module.exports = Service;