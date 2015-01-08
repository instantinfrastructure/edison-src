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

/** @module main */

'use strict';

var ConfigManager = require("./core/config-manager.js");
ConfigManager.init(__dirname, "config.json");

/**
 * Class to create service instances ({@tutorial service})
 */
var Service = require("./core/Service.js");

/**
 * Class to create client instances ({@tutorial client})
 */
var Client = require("./core/Client.js");

/**
 * Allows clients to search for services on the network; allows services
 * to advertise themselves on the network. Uses mDNS.
 */
exports.ServiceDirectory = require("./core/ServiceDirectory.js");

/**
 * The main config object for the library
 * @type {object} see {@link lib/config.json}, {@link module:configManager}
 */
exports.config = ConfigManager.config;

/**
 * Defines a service. Used in creating a service or when a client
 * wants to connect to a service.
 * @see {@tutorial service-spec-query}
 */
exports.ServiceSpec = require("./core/ServiceSpec.js");

/**
 * Class to represent the "kind" of services a client is search for in the LAN
 * @type {ServiceQuery|exports} {@link ServiceQuery}
 */
exports.ServiceQuery = require("./core/ServiceQuery.js");

/**
 * Simple function to test if iotkit-comm is installed correctly
 * @returns {string} A simple greeting
 */
exports.sayhello = function ()
{
  return "Hello Edison user!";
};

/**
 * Create a service and advertise it over the LAN
 * @param serviceSpec {object} JSON service spec object ({@tutorial service-spec-query})
 * @param serviceCreatedCallback {module:main~serviceCreatedCallback} returns created service instance ({@tutorial service})
 */
exports.createService = function (serviceSpec, serviceCreatedCallback) {
  var service = new Service(serviceSpec);

  if (!service.spec.advertise || service.spec.advertise.locally) {
    var serviceDirectory = new exports.ServiceDirectory();
    serviceDirectory.advertiseService(service.spec);
    service.setDirectory(serviceDirectory);
  }

  serviceCreatedCallback(service);
};
/**
 * @callback module:main~serviceCreatedCallback
 * @param service {object} instance of newly created service ({@tutorial service}).
 * Decide if they want to connect to this service or not ({@tutorial service-record}).
 */

/**
 * Create a client instance once a queried service is discovered.
 * @param serviceQuery {object} describes the "kind" of services the app wants to
 * look for ({@tutorial service-spec-query})
 * @param clientCreatedCallback {module:main~clientCreatedCallback} if app returned true as a result of the
 * serviceFilter callback, then this callback returns the newly created client instance that is initialized
 * to communicate with the service that was just found. * @param clientCreatedCallback {module:main~clientCreatedCallback} if app returned true as a result of the
 * serviceFilter callback, then this callback returns the newly created client instance that is initialized
 * to communicate with the service that was just found.
 * @param serviceFilter {module:main~serviceFilter} - called when a matching service is found. App must return 'true'
 * if it wants to communicate with this service.
 */
exports.createClient = function (serviceQuery, clientCreatedCallback, serviceFilter) {
  // service is fully specified, connect directly
  if (serviceQuery.address && serviceQuery.port) {
    clientCreatedCallback(new Client(serviceQuery));
    return;
  }

  // search for service
  var serviceDirectory = new exports.ServiceDirectory();
  serviceDirectory.discoverServices(serviceQuery, function(serviceSpec) {
    if (!serviceFilter || serviceFilter(serviceSpec))
      clientCreatedCallback(new Client(serviceSpec, serviceDirectory));
  });
};
/**
 * @callback module:main~serviceFilter
 * @param serviceRecord {object} a service record matching the query (see {@tutorial service-record})
 * @returns {boolean} Return 'true' if service described by serviceRecord is acceptable, 'false' otherwise
 */
/**
 * @callback module:main~clientCreatedCallback
 * @param client {object} client instance connected to the newly found service. A newly found service is one whose
 * record was accepted by {@link module:main~serviceFilter}.
 */
