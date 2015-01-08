/*
 * Distributed Thermostat sample program through iotkit-comm API
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

/** @file dashboard.c

    Sample client program of dashboard based on ZMQ Pub/Sub
*/

#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include "iotkit-comm.h"
#include "util.h"

/** Callback function. To to be invoked when it receives any messages for the subscribed topic
* @param message the message received from service/publisher
* @param context a context object
 */
void clientMessageCallback(char *message, Context context) {

  // remove the topic from the message content (the temperature)
  char *temperature = strstr(message,":");
  if (temperature != NULL) {
        temperature++;
        fprintf(stdout,"Received mean temperature : %s\n", temperature);
  }
}

/** Callback function. Once the service is discovered this callback function will be invoked
* @param queryDesc the query description object
* @param error_code the error code
* @param commHandle the communication handle used to invoke the interfaces
 */
void subDiscoveryCallback(ServiceQuery *queryDesc, int32_t error_code, CommHandle *commHandle) {

    int (**subscribe)(char *);
    int (**receive)(void (*)(char *, Context));
    if (commHandle != NULL) {
        subscribe = commInterfacesLookup(commHandle, "subscribe");
        receive = commInterfacesLookup(commHandle, "receive");
        if (subscribe != NULL && receive != NULL) {
            while (1) { // Infinite Event Loop
                (*subscribe)("mean_temp");
                (*receive)(clientMessageCallback);
                 sleep(2);
            }
        } else {
            fprintf(stderr, "Interface lookup failed\n");
        }
    } else {
        fprintf(stderr, "Comm Handle is NULL\n");
    }
}


/** The starting point. Starts browsing for the given Service name
 */
int main(void) {

    puts("Dashboard reading the mean temperature from thermostat");
    ServiceQuery *query = (ServiceQuery *) parseServiceQuery("./serviceQueries/thermostat-query.json");
    if (query) {
        discoverServicesBlocking(query, subDiscoveryCallback);
    }
    return 0;
}
