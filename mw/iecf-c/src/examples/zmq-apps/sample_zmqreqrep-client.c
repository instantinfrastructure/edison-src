/*
 * ZMQ REQ/REP sample program through iotkit-comm API
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

/** @file sample_zmqreqrep-client.c
    Sample client program of ZMQ requester.
*/

#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include "iotkit-comm.h"
#include "util.h"

/** Callback function. To be invoked when it receives any messages from the Service.
* @param message the message received from service
* @param context a context object
*/
void reqMessageCallback(char *message, Context context) {
    fprintf(stderr,"Message received in Client: %s\n", message);
}

/** Callback function. Once the service is discovered, this callback function will be invoked.

* @param servQuery the client query object
* @param error_code the error code
* @param commHandle the communication handle used to invoke the interfaces
*/
void reqDiscoveryCallback(ServiceQuery *servQuery, int32_t error_code, CommHandle *commHandle) {
    if (commHandle != NULL) {
        int (**send)(char *, Context context);
        int (**receive)(void (*)(char *, Context));
        Context context;

        send = commInterfacesLookup(commHandle, "send");
        receive = commInterfacesLookup(commHandle, "receive");
        if (send != NULL && receive != NULL) {
            while (1) { // Infinite Event Loop
                (*send)("toys",context);
                (*receive)(reqMessageCallback);
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
    puts("Sample program to test the iotkit-comm ZMQ req/rep plugin !!");
    ServiceQuery *query = (ServiceQuery *) parseServiceQuery("./serviceQueries/temperatureServiceQueryZMQREQREP.json");

    if (query) {
        fprintf(stderr,"query host address %s\n",query->address);
        fprintf(stderr,"query host port %d\n",query->port);
        fprintf(stderr,"query service name %s\n",query->service_name);
        discoverServicesBlocking(query, reqDiscoveryCallback);
    }

    return 0;
}
