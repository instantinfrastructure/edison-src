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
 * Tests the zmq plugin using various clients. Clients are either subscribers in the zmq pub/sub model
 * or requesters in the zmq req/rep model. In each case the client first discovers the service (a publisher
 * or replier) and then interacts with it.
 * @module test/zmq
 * @see {@link module:test/zmq~subscriber}
 * @see {@link module:test/zmq~requester}
 * @see {@link module:test/zmq~publisher}
 * @see {@link module:test/zmq~replier}
 */

var path = require('path');
var expect = require('chai').expect;

describe('[zmq]', function () {

  /**
   * ZMQ service that publishes data on topic "mytopic". Used to test
   * a ZMQ subscriber.
   * @see {@link http://zeromq.org}
   * @see {@link module:test/zmq~subscriber}
   */
  function publisher() {
    var iotkit = require('iotkit-comm');
    var path = require('path');

    var spec = new iotkit.ServiceSpec(path.join(__dirname, "resources/specs/1885-temp-service-zmq-pubsub.json"));
    iotkit.createService(spec, function (service) {
      "use strict";

      setInterval(function () {
        "use strict";
        service.comm.publish("mytopic: my message", {});
      }, 300);

    });
  }

  /**
   * ZMQ service that replies to requests. Used to test
   * a ZMQ requester. Any request string results in the reply "hi"
   * @see {@link http://zeromq.org}
   * @see {@link module:test/zmq~replier}
   */
  function replier() {
    var iotkit = require('iotkit-comm');
    var path = require('path');

    var spec = new iotkit.ServiceSpec(path.join(__dirname, "resources/specs/8333-temp-service-zmq-reqrep.json"));
    iotkit.createService(spec, function (service) {
      "use strict";

      service.comm.setReceivedMessageHandler(function(client, msg, context) {
        "use strict";
        service.comm.sendTo(client, "hi");
      });

    });
  }

  before(function () {
    publisher();
    replier();
  });

  // The mdns browser returns all new services it finds. This means, that once it
  // finds a service record, it won't find it again unless that service went down and came back up.
  // Since the service we want to discover is not restarted between tests, just restart the
  // service browser for each test.
  beforeEach(function() {
    var iotkit = require('iotkit-comm');
  });

  describe('#subscriber', function () {
    /**
     * Subscribes to topic "mytopic" from a ZMQ publisher.
     * @see {@link example/zmq-publisher.js}
     * @function module:test/zmq~subscriber
     */
    it("should successfully subscribe to messages from ZMQ publisher",
      function(done) {
        var iotkit = require('iotkit-comm');
        var query = new iotkit.ServiceQuery(path.join(__dirname, "resources/queries/temp-service-query-zmq-pubsub.json"));
        iotkit.createClient(query, function (client) {
            "use strict";

            client.comm.subscribe("mytopic");

            client.comm.setReceivedMessageHandler(function(message, context) {
              "use strict";
              expect(context.event).to.equal("message");
              expect(message.toString()).to.equal("mytopic: my message");

              // close client connection
              client.comm.done();
              done();
            });
          },
          function (serviceSpec) {
            return true;
          });
      });
  }); // end #subscriber

  describe("#requester", function() {

    /**
     * Sends a request string to ZMQ replier. Any request should result in
     * the reply "hi".
     * @see {@link example/zmq-replier.js}
     * @function module:test/zmq~requester
     */
    it("should successfully receive reply from a ZMQ replier", function(done) {
      var iotkit = require('iotkit-comm');
      var query = new iotkit.ServiceQuery(path.join(__dirname, "resources/queries/temp-service-query-zmq-reqrep.json"));
      iotkit.createClient(query,
        function (client) {
          client.comm.setReceivedMessageHandler(function(message, context) {
            "use strict";
            expect(context.event).to.equal("message");
            expect(message.toString()).to.equal("hi");
            client.comm.done();
            done();
          });
          client.comm.send("hello");
        },
        function (serviceSpec) {
          return true;
        }
      );
    });
  }); // end #requester

});