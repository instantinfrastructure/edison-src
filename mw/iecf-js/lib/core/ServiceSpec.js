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
var ServiceQuery = require("./ServiceQuery.js");

function ServiceSpec(source, otherAddresses) {

  // inheritance
  ServiceQuery.call(this, source);

  // check port
  if (!this.sourceObj.port) {
    throw new Error("Service must have a port number.");
  }
  if (typeof this.sourceObj.port !== 'number') {
    throw new Error("Must specify service port as a number.");
  }
  this.port = this.sourceObj.port;

  // check address
  if (this.sourceObj.address) {
    if (typeof this.sourceObj.address !== 'string') {
      throw new Error("Address must be a string (IPv4).");
    }
    this.address = this.sourceObj.address;
  }

  // check other addresses; this.address may be set by super class ServiceQuery
  if (otherAddresses) {
    if (!Array.isArray(otherAddresses))
      throw new Error("otherAddresses field must be an array (of IPv4 addresses).");
    this.otherAddresses = otherAddresses;
    if (!this.address) { // no address given
      this.address = this.otherAddresses[0];
    }
  }

  // check advertise (how to advertise this service)
  if (this.sourceObj.advertise) {
    if (typeof this.sourceObj.advertise.locally === 'undefined') {
      throw new Error("Missing boolean property 'advertise.locally. " +
        "Service needs to state if it must be locally advertised or not.");
    }
    if (typeof this.sourceObj.advertise.locally !== 'boolean') {
      throw new Error("advertise.locally must be a Boolean property.");
    }
    if (typeof this.sourceObj.advertise.cloud === 'undefined') {
      throw new Error("Missing boolean property 'advertise.cloud. " +
        "Service needs to state if it must be advertised in the cloud or not.");
    }
    if (typeof this.sourceObj.advertise.locally !== 'boolean') {
      throw new Error("advertise.cloud must be a Boolean property.");
    }
    this.advertise = this.sourceObj.advertise;
  }

  if (!this.advertise && !(this.address && this.port)
    || this.advertise && this.advertise.locally) { // service might be advertised on LAN
    if (!this.name) // service does not have a name
      throw new Error("ServiceSpec.name must be a non-empty string. Preferably something user-friendly.");
  }

  // check communication parameters (goes to plugin)
  if (this.sourceObj.type_params) {
    if (typeof this.sourceObj.type_params !== 'object') {
      throw new Error("Communication params field must be an object. It should contain name/value pairs.");
    }
    this.type_params = this.sourceObj.type_params;
  }
}

// inheritance
ServiceSpec.prototype = Object.create(ServiceQuery.prototype);
ServiceSpec.prototype.constructor = ServiceSpec;

module.exports = ServiceSpec;