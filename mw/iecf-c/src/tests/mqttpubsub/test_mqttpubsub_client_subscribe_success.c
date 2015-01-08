/*
 * Tests mqtt subscribe
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

/** @file test_mqttpubsub_client_subscribe_success.c

 */

#include <stdio.h>
#include <MQTTAsync.h>
#include "../../lib/libiotkit-comm/iotkit-comm.h"

bool receivedMessage = false;

void message_callback(char *message, Context context) {
    printf("Message received:%s\n", message);
    receivedMessage = true;
}

int main(void) {
    ServiceQuery *serviceQuery = (ServiceQuery *)malloc(sizeof(ServiceQuery));
    if (serviceQuery != NULL) {
        serviceQuery->address = "localhost";
        serviceQuery->port = 1883;
        int result = init(serviceQuery);
        if (result == MQTTASYNC_SUCCESS) {
            puts("Successfully Connected to an MQTT Broker");
            receive(message_callback);
            result = subscribe("/foo"); //correct topic
            if (result == 0) {
                puts("Subscribed Successfully");
                puts("Waiting for messages on subscribed topic");
                Context context;
                context.name = "topic";
                context.value = "/foo";
                send("This is a test message", context);
                int timerIndex = 0;
                while(timerIndex < 10 && !receivedMessage) { // Wait for 10 seconds
                    sleep(1);
                    timerIndex ++;
                }
                if(receivedMessage) {
                    puts("Test Passed: Successfully received message on subscribed topic");
                    exit(EXIT_SUCCESS);
                } else {
                    puts("Test Failed: No message received on subscribed topic");
                }
            } else {
                puts("Test Failed: Could not subscribe");
            }
        } else {
            puts("Test Failed: Could not connect to MQTT Broker");
        }
        done();
        free(serviceQuery);
    }
    exit(EXIT_FAILURE);
}
