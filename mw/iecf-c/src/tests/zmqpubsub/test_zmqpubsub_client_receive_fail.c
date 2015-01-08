/*
 * ZMQ PUB/SUB test program through iotkit-comm API
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

/** @file test_zmqpubsub_client_subscribe_success.c

 */

#include <stdio.h>
#include <zmq.h>
#include <zmq_utils.h>
#include <signal.h>
#include "../../lib/libiotkit-comm/iotkit-comm.h"

void handler() {
    puts("Didn't receive message");
    exit(EXIT_SUCCESS);
}

int main(void) {
    ServiceQuery *serviceQuery = (ServiceQuery *)malloc(sizeof(ServiceQuery));
    if (serviceQuery != NULL) {
        serviceQuery->address = "localhost";
        serviceQuery->port = 5563;
        int result = init(serviceQuery);
        if (result == -1)
            puts("client init failed");
        result = subscribe("flower");
        if (result == -1)
            puts("client subscribe failed");
        /* Establish a handler for SIGALRM signals. */
        signal(SIGALRM, handler);
        /* Set an alarm to go off*/
        alarm(3);
        result = receive(handler);
        if (result == -1)
            puts("receive failed");
        done();
        free(serviceQuery);
    }
    exit(EXIT_FAILURE);
}
