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
 * @file A dummy temperature sensor. Randomly chooses a reasonable
 * integer to publish as temperature. The thermostat looks for these
 * sensors (on the same LAN) and subscribes to the temperatures they
 * publish. Those temperatures are then accumulated into a mean
 * temperature.
 * @see {@link example/distributed-thermostat/thermostat.js}
 */
var iotkit = require('iotkit-comm');

// read the spec that describes the temperature sensing service
var spec = new iotkit.ServiceSpec('temperature-sensor-spec.json');

// would normally use 'port' number in spec, however, in this case, makes
// it easy to run many temperature sensors on the same local machine (low
// likelihood of 'address in use' errors)
spec.port = getRandomInt(8000, 60000);

// create the temperature sensing service described by the spec
// read above.
iotkit.createService(spec, function (service) {

  // periodically publish sensed temperature. The thermostat
  // will eventually find this sensor and subscribe to the
  // temperature being published.
  setInterval(function () {
    service.comm.publish("mytemp: " + getRandomInt(60, 90));
  }, 1000);
});

function getRandomInt(min, max) {
  return Math.floor(Math.random() * (max - min + 1)) + min;
}