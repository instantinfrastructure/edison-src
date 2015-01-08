/*
 * Sample program to demonstrate MQTT Async subscribe feature through iotkit-comm API
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
 * @file publisher.c
 * @brief Sample to demonstrate MQTT publisher through iotkit-comm API.
 *
 * Provides features to connect to an MQTT Broker and publish a topic.
 */

#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include "iotkit-comm.h"
#include "util.h"

#ifndef DEBUG
    #define DEBUG 0
#endif

ServiceSpec *srvSpec = NULL;
int msgnumber = 0;

/**
 * @name Callback to handle the communication
 * @brief Handles the communication with an MQTT broker once the connection is established.
 * @param[in] error_code specifies the error code is any
 * @param[in] serviceHandle is the client object initialized with the required APIs
 *
 * Handles the communication such as publishing data to an MQTT broker once the connection is established.
 */
void callback(void *handle, int32_t error_code, void *serviceHandle) {
    Context context;
    char msg[256];

    if (serviceHandle != NULL) {
        CommHandle *commHandle = (CommHandle *)serviceHandle;

        int (**send)(char *, Context context);

        send = commInterfacesLookup(commHandle, "send");
        if (send == NULL) {
            printf("Function \'send\' is not available; please verify the Plugin documentation !!\n");
            return;
        }

        context.name = "topic";
        context.value = "/foo";

        while(1) {  // Infinite Event Loop
            sprintf(msg, "This is a test message %d", msgnumber++);
            printf("Publishing msg:%s\n", msg);

            (*send)(msg, context);
            sleep(2);
        }
    }
}

/**
 * @name Starts the application
 * @brief Starts the application to publish for a topic.
 *
 * Establishes the connection with an MQTT broker.
 */
int main(void) {

    puts("Sample program to publish topic \'/foo\' !!");

    srvSpec = (ServiceSpec *) parseServiceSpec("./serviceSpecs/temperatureServiceMQTT.json");

    #if DEBUG
        printf("status:%d:service_name:%s:address:%s:port:%d:name:%s:protocol:%s\n", srvSpec->status, srvSpec->service_name, srvSpec->address, srvSpec->port, srvSpec->type.name, srvSpec->type.protocol);
    #endif

    if (srvSpec) {
        if (!advertiseService(srvSpec)) {
            fprintf(stderr, "Failed to advertise the service\n");

            return 0;
        }

        createClientForGivenService(srvSpec, callback);
    }

    return 0;
}
