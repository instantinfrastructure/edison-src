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
 * Tests if the  library is correctly installed. File is named
 * aaa-install.js so that when the test runner ('mocha') sorts test files, this
 * one will be run first. If any test in this suite fails, ignore errors
 * in the rest of the tests.
 * @module test/install
 * @see module:test/install~configuration
 * @see module:test/install~nohome
 */

var expect = require('chai').expect;

describe('[install]', function () {
  beforeEach(function () {
    // will need to reload 'iotkit-comm' each time, instead of using
    // cached version like node would prefer.
    var name = require.resolve('iotkit-comm');
    delete require.cache[name];
  });

  /**
   * @function module:test/install~configuration
   */
  it("should verify if  library is correctly installed", function (done) {
    var iotkit = require('iotkit-comm');
    expect(iotkit.sayhello()).to.equal("Hello Edison user!");
    done();
  });

  /**
   * @function module:test/install~nohome
   */
  it("should not fail if HOME environment variable is not present", function (done) {
    var savedhome = "";
    if (process.env.HOME) {
      savedhome = process.env.HOME;
      delete process.env.HOME;
    }

    var iotkit = require('iotkit-comm');

    if (savedhome)
      process.env.HOME = savedhome;

    done();
  });
});