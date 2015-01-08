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

/** @file test_zmqpubsub_client_socket_connect_success.c

 */

#include <stdio.h>
#include <zmq.h>
#include <zmq_utils.h>
#include "../../lib/libiotkit-comm/iotkit-comm.h"

int main(void) {
    ServiceQuery *serviceQuery = (ServiceQuery *)malloc(sizeof(ServiceQuery));
    if (serviceQuery != NULL) {
        serviceQuery->address = "localhost";
        serviceQuery->port = 1234;
        int result = init(serviceQuery);
        done();
        free(serviceQuery);
        if (result == 0) {
            puts("Subscriber Socket Successfully Connected");
            exit(EXIT_SUCCESS);
        } else {
            puts("Failed: Subscriber Socket Initialization");
        }
    }
    exit(EXIT_FAILURE);

}
