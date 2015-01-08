/*
 * iotkit-comm API
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

/** @file test_iotkit-comm_createservice_fail.c

*/

#include <stdio.h>
#include <zmq.h>
#include <zmq_utils.h>
#include "../../lib/libiotkit-comm/iotkit-comm.h"

int main(void) {
    ServiceSpec *serviceSpec = (ServiceSpec *) parseServiceSpec("./invalidtemperatureServiceZMQPUBSUB.json");
    if (serviceSpec && createService(serviceSpec)) {
         puts("Success: Created Service");
         exit(EXIT_FAILURE);
    } else {
         puts("Failed: Create Service");
         exit(EXIT_SUCCESS);
    }
}
