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

/** @file sample_zmqreqrep-service.c
    Sample service program of ZMQ replier.
*/

#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include "iotkit-comm.h"
#include "util.h"

/** Callback function. To to be invoked when it receives any messages from the Client.
* @param client the client object
* @param message the message received from client
* @param context a context object
*/
void repMessageCallback(void *client, char *message, Context context) {
    fprintf(stderr,"Message received in Server: %s\n", message);
}

/** Callback function. Once the service is advertised, this callback function will be invoked.

* @param servSpec the service specification object
* @param error_code the error code
* @param serviceHandle the communication handle used to invoke the interfaces
*/
void repAdvertiseCallback(ServiceSpec *servSpec, int32_t error_code,CommHandle *serviceHandle) {
    if (serviceHandle != NULL) {
        void *client = NULL;
        Context context;
        void (**sendTo)(void *, char *, Context context);
        int (**receive)(void (*)(void *, char *, Context context));

        sendTo = commInterfacesLookup(serviceHandle, "sendTo");
        receive = commInterfacesLookup(serviceHandle, "receive");
        if (sendTo != NULL && receive != NULL) {
            while(1) {  // Infinite Event Loop
                (*sendTo)(client,"train bike car",context);
                (*receive)(repMessageCallback);
                sleep(2);
            }
        } else {
            puts("Interface lookup failed");
        }
    } else {
        puts("\nComm Handle is NULL\n");
    }
}

/** The starting point. Starts to advertise the given Service.
*/
int main(void) {
    puts("Sample program to test the iotkit-comm ZMQ req/rep plugin !!");
    ServiceSpec *serviceSpec = (ServiceSpec *) parseServiceSpec("./serviceSpecs/temperatureServiceZMQREQREP.json");

    if (serviceSpec) {
        advertiseServiceBlocking(serviceSpec, repAdvertiseCallback);
    }

    return 0;
}
