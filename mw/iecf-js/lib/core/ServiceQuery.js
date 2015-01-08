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
var fs = require('fs');

/**
 * The service query object. Users provide this to find the desired service records returned by mDNS.
 * @param rawQueryObj {object} ({@tutorial service-spec-query})
 * @constructor module:client~ServiceQuery
 */
function ServiceQuery(source) {
  if (typeof source === "string") {
    if (fs.existsSync(source)) { // file path
      try {
        this.sourceObj = JSON.parse(fs.readFileSync(source));
      } catch (err) {
        throw new Error("Invalid JSON in file.");
      }
    } else { // possibly json string
      try {
        this.sourceObj = JSON.parse(source);
      } catch (err) {
        throw new Error("argument to " + this.constructor.name + " must be a valid JSON file, string, or object. "
          + "(argument is " + source + ").");
      }
    }
  } else if (typeof source === "object") {
    this.sourceObj = source;
  } else {
    throw new Error("Incorrect argument type: first argument must be a 'string' or an 'object'.");
  }

  // make sure service query does not contain address or port
  if (this.constructor.name === "ServiceQuery" && (this.sourceObj.address || this.sourceObj.port)) {
    throw new Error("A service query cannot contain an address or port number; use service specification instead.");
  }

  // check type
  if (!this.sourceObj.type || !this.sourceObj.type.name || !this.sourceObj.type.protocol) {
    throw new Error("Must specify service type; type.name; and type.protocol.");
  }
  if (this.sourceObj.type.subtypes) {
    if (!Array.isArray(this.sourceObj.type.subtypes)) {
      throw new Error("Subtypes of a service type must be in an array.");
    }
    if (this.sourceObj.type.subtypes.length > 1) {
      throw new Error("More than one subtype is not supported at this time.");
    }
  }
  this.type = this.sourceObj.type;

  // check name
  if (this.sourceObj.name) {
    if (typeof this.sourceObj.name !== "string") {
      throw new Error("Must specify service name as a non-zero string.");
    }
    this.nameRegEx = new RegExp(this.sourceObj.name);
    this.name = this.sourceObj.name;
  }

  // check properties
  var properties = null;

  // 'properties' comes from JSON service query, whereas txtRecord comes from a mdns service record
  // This object could be initialized from any of these sources. So any differences, need to be
  // handled appropriately.
  if (this.sourceObj.properties && this.sourceObj.txtRecord)
    throw new Error("Can't have both 'properties' and 'txtRecord' field. They are the same thing.");

  if (this.sourceObj.properties)
    properties = this.sourceObj.properties;
  else if (this.sourceObj.txtRecord)
    properties = this.sourceObj.txtRecord;

  if (properties) {
    if (typeof properties !== 'object' || Array.isArray(properties)) {
      throw new Error("Must specify service properties as an object containing name/value pairs.");
    }
    this.properties = properties;
  }
}

module.exports = ServiceQuery;