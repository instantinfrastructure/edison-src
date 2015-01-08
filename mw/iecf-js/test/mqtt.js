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
 * Tests the MQTT plugin using various clients. The clients assume that an MQTT broker already exists. For this
 * test suite, the mosquitto broker and a mini broker built using the iotkit library is used. The suite features clients
 * that connect directly to a broker at a known address, clients that discover a broker and connect to it, and clients
 * that connect to a broker that is being used as a proxy by another client (Essentially, this client is acting like a
 * service by using the broker as a proxy.)
 * @module test/mqtt
 * @see {@link module:test/mqtt~direct_publisher}
 * @see {@link module:test/mqtt~direct_subscriber}
 * @see {@link module:test/mqtt~discover_publisher}
 * @see {@link module:test/mqtt~discover_subscriber}
 * @see {@link module:test/mqtt~discover_direct}
 * @see {@link module:test/mqtt~client_as_a_service}
 * @see {@link module:test/mqtt~subscribe_to_client_as_a_service}
 * @see {@link module:test/mqtt~mqttMiniBroker}
 */

var path = require('path');
var expect = require('chai').expect;

describe('[mqtt]', function () {
  /**
   * A MQTT mini broker that is used for testing MQTT clients.
   * @see {@link https://github.com/adamvr/MQTT.js}
   * @see {@link module:test/mqtt~discover_publisher}
   * @see {@link module:test/mqtt~discover_subscriber}
   * @see {@link module:test/mqtt~discover_direct}
   */
  function mqttMiniBroker() {
    var path = require('path');
    var iotkit = require('iotkit-comm');

    var spec = new iotkit.ServiceSpec(path.join(__dirname, "resources/specs/1889-mqtt-mini-broker-spec.json"));
    iotkit.createService(spec, function (service) {
      var clients = {};

      service.comm.setReceivedMessageHandler(function(client, msg, context) {
        "use strict";
        switch (context.event) {
          case 'connect':
            clients[msg.clientId] = client;
            service.comm.sendTo(client, msg, {ack: 'connack'});
            break;
          case 'publish':
            for (var clientId in clients) {
              if (!clients.hasOwnProperty(clientId))
                continue;
              service.comm.sendTo(clients[clientId], msg);
            }
            break;
          case 'subscribe':
            service.comm.sendTo(client, msg, {ack: 'suback'});
            break;
          case 'close':
            for (var clientId in clients) {
              if (!clients.hasOwnProperty(clientId))
                continue;
              if (clients[clientId] == client) {
                delete clients[clientId];
              }
            }
            break;
          case 'pingreq':
            service.comm.sendTo(client, msg, {ack: 'pingresp'});
            break;
          case 'disconnect':
          case 'error':
            service.comm.manageClient(client, {action: 'endstream'});
            break;
          default:
            console.log(context.event);
        }
      });
    });
  }

  before(function () {
    "use strict";
    mqttMiniBroker();
  });

  describe('#direct', function () {
    /**
     * Publishes data directly to (i.e. without discovering) an MQTT broker (e.g. mosquitto) on topic 'mytopic'.
     * Preconditions are that the MQTT broker is running on a known address and port, and the broker specification
     * file {@link example/serviceSpecs/mqtt-borker-spec.json} has the address and port fields
     * correctly set. No changes are needed if this program is run on the Edison. Each Edison comes with a
     * running broker and the address and port fields of the specification file are set to '127.0.0.1' and '1883'
     * (see {@tutorial service-spec-query}).
     * @function module:test/mqtt~direct_publisher
     */
    it("should successfully publish to mosquitto broker",
      function(done) {
        var iotkit = require('iotkit-comm');

        var spec = new iotkit.ServiceSpec(path.join(__dirname, "resources/specs/1883-mqtt-broker-spec.json"));
        iotkit.createClient(spec, function (client) {
          setInterval(function () {
            "use strict";
            client.comm.send("my other message", {topic: "mytopic"});
          }, 200);
          done();
        });
      });

    /**
     * Subscribes to data directly from (i.e. without discovering) an MQTT broker (e.g. mosquitto) on topic 'mytopic'.
     * Preconditions are that the MQTT broker is running on a known address and port, and the broker specification
     * file {@link example/serviceSpecs/mqtt-borker-spec.json} has the address and port fields
     * correctly set. No changes are needed if this program is run on the Edison. Each Edison comes with a
     * running broker and the address and port fields of the specification file are set to '127.0.0.1' and '1883'
     * (see {@tutorial service-spec-query}).
     * @function module:test/mqtt~direct_subscriber
     */
    it("should successfully subscribe to data from mosquitto broker",
      function(done) {
        var iotkit = require('iotkit-comm');

        var spec = new iotkit.ServiceSpec(path.join(__dirname, "resources/specs/1883-mqtt-broker-spec.json"));
        iotkit.createClient(spec, function (client) {
          client.comm.subscribe("mytopic");
          client.comm.setReceivedMessageHandler(function(message, context) {
            "use strict";
            expect(context.event).to.equal("message");
            expect(context.topic).to.equal("mytopic");
            expect(message).to.equal("my other message");
            client.comm.done();
            done();
          });
        });
      });
  });

  describe('#discover_direct', function () {
    /**
     * Shows how to find and connect to an MQTT service or broker running on the LAN. Uses single client. First,
     * call discoverServices with the appropriate service query ({@tutorial service-spec-query}) and then
     * call createClient with the found service specification ({@tutorial service-spec-query}).
     * The preconditions are that a network connections exists, that an MQTT service (or broker) is running
     * on the LAN, and that the service is advertising appropriate service information. To run a sample of such
     * a service see {@link example/mqtt-mini-broadcast-broker.js}
     * @function module:test/mqtt~discover_direct
     */
    it("should successfully discover and interact with mini broker using one client",
      function(done) {
        var iotkit = require('iotkit-comm');

        var serviceDirectory = new iotkit.ServiceDirectory();
        var query = new iotkit.ServiceQuery(path.join(__dirname, "resources/queries/mqtt-mini-broker-query.json"));

        // explicitly discover service first. This can be skipped; see other tests that show
        // how to discover and connect automatically.
        serviceDirectory.discoverServices(query, function (serviceSpec) {
          // once service has been discovered, connect to it
          iotkit.createClient(serviceSpec, function (client) {

            // client subscribes to a topic
            client.comm.subscribe("mytopic");

            client.comm.setReceivedMessageHandler(function(message, context) {
              "use strict";
              // if client receives message it published on the topic
              // it subscribed to, then test passes.
              expect(context.event).to.equal("message");
              expect(context.topic).to.equal("mytopic");
              expect(message).to.equal("my message");

              // close client connection
              client.comm.done();
              done();
            });

            // client publishes to topic it previously subscribed to
            client.comm.send("my message", {topic: "mytopic"});

          });

        });
      });
  });

  describe('#discover', function() {

    /**
     * Discovers and publishes data to the mini broadcast broker.
     * Prerequisite is that the mini broker is running on the LAN.
     * @function module:test/mqtt~discover_publisher
     * @see {@link example/mqtt-mini-broadcast-broker.js}
     */
    it("should discover and publish data to mini broker",
      function(done) {
        var iotkit = require('iotkit-comm');

        var query = new iotkit.ServiceQuery(path.join(__dirname, "resources/queries/mqtt-mini-broker-query.json"));
        iotkit.createClient(query, function (client) {
            setInterval(function () {
              client.comm.send("my other other message", {topic: "mytopic"});
            }, 200);

            done();
          },
          function (serviceSpec) {
            "use strict";
            return true;
          });
      });

    /**
     * Discovers and subscribes to data from mini broadcast broker. Prerequisite is that
     * the mini broker is running on the LAN.
     * @function module:test/mqtt~discover_subscriber
     * @see {@link example/mqtt-mini-broadcast-broker.js}
     */
    it("should discover and subscribe to data from mini broker",
      function (done) {
        var iotkit = require('iotkit-comm');
        var query = new iotkit.ServiceQuery(path.join(__dirname, "resources/queries/mqtt-mini-broker-query.json"));
        iotkit.createClient(query, function (client) {
            client.comm.subscribe("mytopic");
            client.comm.setReceivedMessageHandler(function(message, context) {
              "use strict";
              expect(context.event).to.equal("message");
              expect(context.topic).to.equal("mytopic");
              expect(message).to.equal("my other other message");

              // close client connection
              client.comm.done();
              done();
            });
          },
          function (serviceSpec) {
            return true;
          });
      });
  });

  describe('#advertise-broker-as-service', function() {
    /**
     * Client publishes temperature data to a local broker and advertises itself as a temperature service.
     * The client does this by using the broker as a proxy. Make sure the local broker is running on port '1883'.
     * Note: On the Edison, a local broker should already be running on port '1883'.
     * @function module:test/mqtt~client_as_a_service
     */
    it("should allow a client to act like a service by advertising the broker (proxies for client)",
      function(done) {
        var iotkit = require('iotkit-comm');
        var serviceDirectory = new iotkit.ServiceDirectory();
        var spec = new iotkit.ServiceSpec(path.join(__dirname, "resources/specs/1883-temp-service-via-broker.json"));
        serviceDirectory.advertiseService(spec);
        iotkit.createClient(spec, function (client) {
          setInterval(function () {
            client.comm.send("30 deg F", {topic: spec.name});
          }, 200);
          done();
        });
      });

    /**
     * Client subscribes to temperature data from a temperature service. The temperature service in this case
     * is a client that is acting like a service by advertising the broker on the LAN as its proxy. This client, the
     * one that is subscribing, is oblivious of that fact.
     * @function module:test/mqtt~subscribe_to_client_as_a_service
     * @see {@link module:test/mqtt~client_as_a_service}
     */
    it("should allow a client to subscribe to data from a client acting as a service (broker is proxying)",
      function (done) {
        var iotkit = require('iotkit-comm');

        var query = new iotkit.ServiceQuery(path.join(__dirname, "resources/queries/temp-service-query-mqtt.json"));
        iotkit.createClient(query, function (client) {
          client.comm.subscribe(client.spec.name);
          client.comm.setReceivedMessageHandler(function (message, context) {
            "use strict";
            expect(context.event).to.equal("message");
            expect(context.topic).to.equal(client.spec.name);
            expect(message).to.equal("30 deg F");

            // close client connection
            client.comm.done();
            done();
          }, function (serviceSpec) {
            return true;
          });
        });
      });
  });
});