/*
 * MDNS through iotkit-comm API
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

/** @file test_mdns_advertiseservice_success.c

*/

#include <stdio.h>
#include <zmq.h>
#include <zmq_utils.h>
#include "../../lib/libiotkit-comm/iotkit-comm.h"


/** Callback function. Once the service is discovered this callback function will be invoked
* @param servQuery the service query object
* @param error_code the error code
* @param commHandle the communication handle used to invoke the interfaces
 */
void subDiscoveryCallback(ServiceQuery *servQuery, int32_t error_code, CommHandle *serviceHandle) {
     if (serviceHandle != NULL) {
         puts("Success: Service Advertised");
         exit(EXIT_SUCCESS);
     } else {
         puts("Fail: Service Not Advertised");
         exit(EXIT_FAILURE);
     }
}

int main(void) {
    ServiceSpec *serviceSpec = (ServiceSpec *) parseServiceSpec("./temperatureServiceZMQPUBSUB.json");
    if (serviceSpec) {
        puts("Waiting for the Service to Advertise");
        advertiseServiceBlocking(serviceSpec, subDiscoveryCallback);
    }
    return 0;
}
