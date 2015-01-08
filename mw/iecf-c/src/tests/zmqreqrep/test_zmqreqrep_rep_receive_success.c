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

void handler(void *client,char *message,Context context) {
    printf("Received message: %s\n",message);
    exit(EXIT_SUCCESS);
}

int main(void) {
    ServiceSpec *serviceSpec = (ServiceSpec *)malloc(sizeof(ServiceSpec));
    if (serviceSpec != NULL) {
        serviceSpec->address = "127.0.0.1";
        serviceSpec->port = 1234;
        init(serviceSpec);
        void *ctx = zmq_ctx_new();
        void *req = zmq_socket(ctx, ZMQ_REQ);
        int rc = zmq_connect(req, "tcp://127.0.0.1:1234");
        if (rc == -1)
            puts("client connect failed");
        //  Send message from client to server
        rc = zmq_send(req, "rose", 4, 0);
        if (rc == -1)
            puts("client send failed");
        puts("waitng for message");
        receive(handler);
        done();
        free(serviceSpec);
        zmq_close(req);
        zmq_ctx_term(ctx);
    }
    exit(EXIT_FAILURE);
}
