/*
 * ZMQ REQ/REP test program through iotkit-comm API
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

/** @file test_zmqreqrep_rep_sendTo_fail.c

This file tests whether ZMQ Responder socket fails while sending message.
*/

#include <stdio.h>
#include <zmq.h>
#include <zmq_utils.h>
#include "../../lib/libiotkit-comm/iotkit-comm.h"

int main(void) {
    ServiceSpec *serviceSpec = (ServiceSpec *)malloc(sizeof(ServiceSpec));
    if (serviceSpec != NULL) {
        serviceSpec->address = "127.0.0.1";
        serviceSpec->port = 1234;
        init(serviceSpec);
        int result = sendTo(NULL,"Hello World",NULL);
        if (result == 0) {
            puts("Sended Message Successfully");
            done();
            free(serviceSpec);
        } else {
            puts("Failed: Sending Message");
            done();
            free(serviceSpec);
            exit(EXIT_SUCCESS);
        }
    }
    exit(EXIT_FAILURE);
}
