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

/** @file test_mdns_createclientforservice_success.c

*/

#include <stdio.h>
#include <zmq.h>
#include <zmq_utils.h>
#include "../../lib/libiotkit-comm/iotkit-comm.h"


void callback(ServiceQuery *queryDesc, int32_t error_code, CommHandle *clientHandle) {
     if (clientHandle != NULL) {
         puts("\nSuccess: Client Created for the Given Service\n");
         exit(EXIT_SUCCESS);
     } else {
         puts("\nFail: Client Creation for the Given Service\n");
         exit(EXIT_FAILURE);
     }
}

int main(void) {
    ServiceQuery *query = (ServiceQuery *)parseServiceQuery("./temperatureServiceQueryZMQPUBSUB.json");
    if (query) {
        query->port = 1080;
        createClientForGivenService(query, callback);
    }
    return 0;
}
