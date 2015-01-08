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

/** @module serviceDirectory */

var mdns = require('mdns2');
var os = require('os');
var dns = require('dns');

var ServiceSpec = require("./ServiceSpec.js");

/**
 * All local services will be reported as running at this IP (even if interfaces might have different addresses).
 */
exports.LOCAL_ADDR = "127.0.0.1";

/**
 * Addresses of all interfaces on this machine.
 */
exports.myaddresses = [];

/**
 * Resolve service names to get address and other details.
 */
exports.mdnsResolverSequence = [
  mdns.rst.DNSServiceResolve(),
  mdns.rst.getaddrinfo({ families: [4] }),
  mdns.rst.makeAddressesUnique()
];

/**
 * The service directory. Uses mDNS
 * @constructor module:serviceDirectory~ServiceDirectory
 */
function ServiceDirectory() {
  if (exports.myaddresses.length == 0)
    setMyAddresses();

  /**
   * Service cache of found service records. Used to eliminate duplicate advertisements.
   */
  this.serviceCache = {};

  /**
   * mDNS service browser instance
   */
  this.browser = null;

  /**
   * mDNS service advertiser instance
   */
  this.advertiser = null;
}

/**
 * Advertise the service on the LAN. Developer can set 'serviceSpec.address'
 * to specify the address at which the service is running. This is useful if
 * the service is intended to run from a given interface (e.g. eth0 instead
 * of eth1).
 * @param serviceSpec {object} - {@tutorial service-spec-query}
 */
ServiceDirectory.prototype.advertiseService = function (serviceSpec) {
  if (serviceSpec.address) {
    dns.lookup(serviceSpec.address, 4, function (err, address, family) {
      if (err) {
        console.log(err);
        throw err;
      }

      if (family != 4) {
        console.log("WARN: Got IPv6 address even when IPv4 was requested. Waiting for an IPv4 address...");
        return;
      }

      serviceSpec.address = address;
    });
  }

  var options, address;
  if (serviceSpec.address) {
    if (serviceSpec.address === exports.LOCAL_ADDR) {
      address = mdns.loopbackInterface();
    } else {
      address = serviceSpec.address;
    }
    options = {txtRecord: serviceSpec.properties, name: serviceSpec.name,
      networkInterface: address};
  } else {
    options = {txtRecord: serviceSpec.properties, name: serviceSpec.name};
  }

  this.advertiser = mdns.createAdvertisement(serviceSpec.type, serviceSpec.port, options);
  this.advertiser.start();
};

/**
 * Find services on the LAN
 * @param serviceQuery {object} - {@tutorial service-spec-query}
 * @param userServiceFilter {ServiceDirectory~userServiceFilter} - user-provided callback to choose the service(s) to connect to
 * @param callback {ServiceDirectory~returnServiceSpec} - return the service spec associated with the chosen service
 */
ServiceDirectory.prototype.discoverServices = function (serviceQuery, callback) {

  if (serviceQuery.constructor.name !== 'ServiceQuery') {
    throw new Error("Invalid argument: must use a ServiceQuery object to discover services.");
  }

  // todo: needs fix: multiple subtypes in the serviceType causes errors.
  // make sure your serviceType contains only *one* subtype
  this.browser = mdns.createBrowser(serviceQuery.type, { resolverSequence: exports.mdnsResolverSequence });

  var that = this;

  this.browser.on('serviceUp', function(service) {
    if (!serviceQueryFilter(serviceQuery, service)) {
      return;
    }

    var filteredServiceAddresses = serviceAddressFilter(service, that.serviceCache);
    if (filteredServiceAddresses.length != 0) {
      var serviceSpec = new ServiceSpec(service, filteredServiceAddresses);
      callback(serviceSpec);
    }
  });

  this.browser.on('serviceDown', function(service) {
    removeServiceFromCache(service, that.serviceCache);
  });

  this.browser.on('serviceChanged', function(service) {
    "use strict";
    // todo: correctly handle service changed. Check if address has changed. Deleting is not the answer since service changed is raised even when serviceup happens.
    //removeServiceFromCache(service);
  });

  this.browser.start();
};
/**
 * @callback ServiceDirectory~userServiceFilter
 * @param serviceRecord {object} A raw service record for the found service. Apps can inspect this record and
 * decide if they want to connect to this service or not ({@tutorial service-record}).
 */
/**
 * @callback ServiceDirectory~returnServiceSpec
 * @param serviceSpec {object} A condensed version of the raw service record for the found service. This is what's
 * passed to the plugins to create client or server objects ({@tutorial service-spec-query}).
 */

/**
 * Shuts down service browser if one is already running.
 */
ServiceDirectory.prototype.stopDiscovering = function () {
  if (this.browser != null) {
    this.browser.stop();
    this.serviceCache = {};
    this.browser = null;
  }
};

/**
 * Get and save all the IP addresses associated with this computer. This will be used to detect services that
 * are running locally and advertising via MDNS.
 */
function setMyAddresses() {
  var ifs = os.networkInterfaces();
  for (var i in ifs) {
    for (var j in ifs[i]) {
      var address = ifs[i][j];
      if (address.family === 'IPv4' && !address.internal) {
        exports.myaddresses.push(address.address);
      }
    }
  }
}

/**
 * Delete a service from the cache. This happens when a service stops advertising itself.
 * @param service {object} The service record associated with the respective service.
 */
function removeServiceFromCache(service, serviceCache) {
  "use strict";
  if (!service.name) {
    console.log("WARN: Cannot remove service. No name in service record. " +
      "The service originally intended to be removed will remain in cache.");
    return;
  }
  delete serviceCache[service.name];
}

/**
 * Check if a service is running locally.
 * @param serviceAddresses {Array} list of IP addresses
 * @returns {boolean} true if local, false otherwise
 */
function serviceIsLocal(serviceAddresses) {
  "use strict";

  if (!serviceAddresses || serviceAddresses.length == 0) {
    return false;
  }

  return serviceAddresses.some(function (serviceAddress) {
    var isLocal = exports.myaddresses.some(function (myaddress) {
      if (serviceAddress === myaddress) {
        return true;
      }
      return false;
    });

    if (isLocal) {
      return true;
    }

    return false;

  });
}

/**
 * Length of matching address prefix. A found service address is compared with a local interface address.
 * @param serviceAddress {string} IP address of service
 * @param myaddress {string} IP address of one of the interfaces on this machine
 * @returns {number} length of the matching prefix
 */
function getMatchingPrefixLen(serviceAddress, myaddress) {
  "use strict";
  var i = 0;
  while(i < serviceAddress.length && i < myaddress.length && serviceAddress[i] == myaddress[i]) {
    i++;
  }

  return i;
}

/**
 * Sort all the addresses found in the service record by the length of the longest prefix match with a local address.
 * Used to suggest to the application the best addresses to use when connecting with the service.
 * @param serviceAddresses {Array} list of addresses found in the mDNS servcice record ({@tutorial service-record})
 * @returns {Array} sorted service addresses
 */
function getAddressesWithLongestPrefixMatch(serviceAddresses) {
  "use strict";
  var resultStore = {};

  serviceAddresses.forEach(function (serviceAddress) {
    exports.myaddresses.forEach(function (myaddress) {
      var matchingPrefixLen = getMatchingPrefixLen(serviceAddress, myaddress);
      if (typeof resultStore[matchingPrefixLen] === 'undefined') {
        resultStore[matchingPrefixLen] = {};
      }
      if (typeof resultStore[matchingPrefixLen] === 'undefined') {
        resultStore[matchingPrefixLen] = {};
      }
      resultStore[matchingPrefixLen][serviceAddress] = true;
    });
  });

  var allPrefixLengths = Object.keys(resultStore);
  if (allPrefixLengths.length == 0) {
    return [];
  }
  allPrefixLengths = allPrefixLengths.map(Math.round);
  allPrefixLengths.sort(function(n1,n2){return n1 - n2});
  return Object.keys(resultStore[allPrefixLengths[allPrefixLengths.length-1]]);
}

/**
 * Eliminate duplicate advertisements, sort service addresses by longest prefix match to local addresses, or
 * return local address for services running on this computer
 * @param service {object} service record
 * @returns {Array} sorted service addresses or an array that contains the local address
 * ({@link ServiceDirectory.exports.LOCAL_ADDR})
 */
function serviceAddressFilter(service, serviceCache) {
  "use strict";

  if (!service.addresses || !service.name) {
    if (!service.name) {
      console.log("WARN: Discovered a service without a name. Dropping.");
    } else {
      console.log("WARN: Discovered a service without addresses. Dropping.");
    }
    return [];
  }

  var notSeenBefore = [];
  service.addresses.forEach(function (address) {
    "use strict";
    if (typeof serviceCache[service.name] === 'undefined') {
      serviceCache[service.name] = {};
    }
    if (!serviceCache[service.name][address]) {
      serviceCache[service.name][address] = true;
      notSeenBefore.push(address);
    }
  });

  if (notSeenBefore.length == 0) {
    return [];
  }

  if (serviceIsLocal(Object.keys(serviceCache[service.name]))) {
    return [ exports.LOCAL_ADDR ];
  }

  if (notSeenBefore.length == 1) {
    return [ notSeenBefore[0] ];
  }

  var longestPrefixMatches = getAddressesWithLongestPrefixMatch(notSeenBefore);
  longestPrefixMatches.sort(); // so we can return addresses in the same order for the same service. Necessary?

  return longestPrefixMatches;
}

/**
 * Check if a found service record matches the application's query.
 * @param query {object} A query for the kind of service records the app is looking for ({@tutorial service-spec-query})
 * @param serviceRecord {object} A service record returned by mDNS ({@tutorial service-record})
 * @returns {boolean} true if service record matches query, false otherwise
 */
function serviceQueryFilter(query, serviceRecord) {
  "use strict";

  if (query.nameRegEx) {
    if (serviceRecord.name) {
      if (query.nameRegEx.test(serviceRecord.name)) {
        return true;
      }
    }
  }

  if (query.properties) {
    // OR
    if (serviceRecord.properties) {
      var found = Object.keys(query.properties).some(function (property) {
        if (serviceRecord.properties[property]) {
          if (serviceRecord.properties[property] === query.properties[property]) {
            return true;
          }
        }
      });
      if (found) {
        return true;
      }
    }
  }

  // MUST contain all fields tested above
  if (!query.nameRegEx && !query.properties) {
    // only a service.type query was issued. Since serrvice.type is a compulsory
    // query attribute to search for services, this service must be of the
    // same type.
    return true;
  }

  return false;
}

module.exports = ServiceDirectory; // must be at the end
