/*
 * ZMQ PUB/SUB sample program through iotkit-comm API
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

/** @file sample_zmqpubsub-client.c
    Sample client program of ZMQ subscriber.
*/

#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include "iotkit-comm.h"
#include "util.h"

/** Callback function. To to be invoked when it receives any messages for the subscribed topic.
* @param message the message received from service/publisher
* @param context a context object
*/
void clientMessageCallback(char *message, Context context) {
    fprintf(stderr,"Message received in Client: %s\n", message);
}

/** Callback function. Once the service is discovered, this callback function will be invoked.
* @param servQuery the service query object
* @param error_code the error code
* @param commHandle the communication handle used to invoke the interfaces
*/
void subDiscoveryCallback(ServiceQuery *servQuery, int32_t error_code, CommHandle *commHandle) {
    int (**subscribe)(char *);
    int (**receive)(void (*)(char *, Context));
    int (**unsubscribe)(char *);
    int i = 0;

    if (commHandle != NULL) {
        subscribe = commInterfacesLookup(commHandle, "subscribe");
        receive = commInterfacesLookup(commHandle, "receive");
        unsubscribe = commInterfacesLookup(commHandle, "unsubscribe");

        if (subscribe != NULL && receive != NULL && unsubscribe != NULL) {
            while (1) {  // Infinite Event Loop
                i++;
                if (i < 10) {
                    (*subscribe)("vehicle");
                    (*receive)(clientMessageCallback);
                } else {
                    if (i == 10) {
                        (*unsubscribe)("vehicle");
                        puts("\nSuccessfully unsubscribed won't receive anymore messages on 'vehicle'\n");
                    }
                }
                sleep(2);
            }
        } else {
            puts("Interface lookup failed");
        }
    } else {
        puts("\nComm Handle is NULL\n");
    }
}

/** The starting point. Starts browsing for the given Service name.
*/
int main(void) {
    puts("Sample program to test the iotkit-comm ZMQ pub/sub plugin !!");
    ServiceQuery *servQuery = (ServiceQuery *) parseServiceQuery("./serviceQueries/temperatureServiceQueryZMQPUBSUB.json");

    if (servQuery) {
        fprintf(stderr,"query host address %s\n",servQuery->address);
        fprintf(stderr,"query host port %d\n",servQuery->port);
        fprintf(stderr,"query service name %s\n",servQuery->service_name);
        discoverServicesBlocking(servQuery, subDiscoveryCallback);
    }

    return 0;
}
