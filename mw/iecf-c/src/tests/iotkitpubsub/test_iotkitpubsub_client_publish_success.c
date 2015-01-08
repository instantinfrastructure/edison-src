/*
 * Tests mqtt publish
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

/** @file test_iotkitpubsub_client_publish_success.c

*/

#include <stdio.h>
#include <MQTTAsync.h>
#include "../../lib/libiotkit-comm/iotkit-comm.h"

int main(void) {
    ServiceQuery *serviceQuery = (ServiceQuery *)malloc(sizeof(ServiceQuery));
    if (serviceQuery != NULL) {
        serviceQuery->address = "localhost";
        serviceQuery->port = 1884;
        int result = init(serviceQuery);
        if (result == MQTTASYNC_SUCCESS) {
            printf("Successfully Connected to an MQTT Broker\n");
            Context context;
            context.name = "topic";
            context.value = "data";
            result = send("{\"n\":\"garage5\",\"t\":\"temperature.v1.0\"}", context);
            if(result != MQTTASYNC_SUCCESS){
                printf("Test Failed: Could not register a sensor\n");
                exit(EXIT_FAILURE);
            }

            sleep(5); // wait for 5 seconds so that sensor registration completes hopefully

            result = send("{\"n\":\"garage5\",\"v\":185}", context);
            if(result == MQTTASYNC_SUCCESS){
                printf("Test Passed: Successfully published message\n");
                exit(EXIT_SUCCESS);
            }
        } else {
            printf("Test Failed: Could not connect to MQTT Broker\n");
        }
        done();
        free(serviceQuery);
    }
    exit(EXIT_FAILURE);
}
