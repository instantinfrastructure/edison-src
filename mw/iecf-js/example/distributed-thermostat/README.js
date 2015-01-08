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
 * A thermostat that collects temperatures from sensors (on the same LAN)
 * as they are published. In this example, sensors (temperature-sensor.js)
 * periodically publish the temperature they are sensing and the thermostat (thermostat.js)
 * subscribes to this temperature data. The thermostat then computes a cumulative moving
 * average of the temperatures received and publishes it for others to subscribe.
 * Specifically, there is a dummy dashboard (dashboard.js) that subscribes to this mean temperature
 * and displays it.
 *
 * @see Tutorial on writing a [distributed application]{@tutorial apps}
 * @see {@link example/distributed-thermostat/temperature-sensor.js}
 * @see {@link example/distributed-thermostat/thermostat.js}
 * @see {@link example/distributed-thermostat/dashboard.js}
 *
 * @module example/distributedThermostat
 */