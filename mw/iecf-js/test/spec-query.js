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
 * Tests if a service query is correctly validated
 * @module test/specAndQuery
 * @see {@link module:test/specAndQuery~wellformedQuery}
 * @see {@link module:test/specAndQuery~wellformedSpec}
 * @see {@link module:test/specAndQuery~NonExistantQuery}
 * @see {@link module:test/specAndQuery~NonExistantSpec}
 * @see {@link module:test/specAndQuery~malformedQuery}
 */

var expect = require('chai').expect;
var path = require('path');

describe('[spec and query]', function () {

  /**
   * @function module:test/specAndQuery~wellformedQuery
   */
  it("should validate a correct query without throwing an error", function() {
    var iotkit = require('iotkit-comm');
    var query = new iotkit.ServiceQuery(path.join(__dirname, "resources/queries/mqtt-mini-broker-query.json"));
    expect(query.name).to.be.a('string');
  });

  /**
   * @function module:test/specAndQuery~wellformedSpec
   */
  it("should validate a correct spec without throwing an error", function() {
    var iotkit = require('iotkit-comm');
    var spec = new iotkit.ServiceSpec(path.join(__dirname, "resources/specs/1889-mqtt-mini-broker-spec.json"));
    expect(spec.name).to.be.a('string');
  });

  /**
   * @function module:test/specAndQuery~malformedQuery
   */
  it("should fail if a query contains both address and port", function(done) {
    var iotkit = require('iotkit-comm');
    try {
      var spec = new iotkit.ServiceQuery(path.join(__dirname, "resources/specs/1883-mqtt-broker-spec.json"));
    } catch (err) {
      done();
    }
  });

  /**
   * @function module:test/specAndQuery~NonExistantQuery
   */
  it("should throw error when query is not present", function(done) {
    var iotkit = require('iotkit-comm');
    try {
      var spec = new iotkit.ServiceQuery(path.join(__dirname, "resources/queries/does-not-exist.json"));
    } catch (err) {
      done();
    }
  });

  /**
   * @function module:test/specAndQuery~NonExistantSpec
   */
  it("should throw error when spec is not present", function(done) {
    var iotkit = require('iotkit-comm');
    try {
      var spec = new iotkit.ServiceSpec(path.join(__dirname, "resources/specs/does-not-exist.json"));
    } catch (err) {
      done();
    }
  });
});