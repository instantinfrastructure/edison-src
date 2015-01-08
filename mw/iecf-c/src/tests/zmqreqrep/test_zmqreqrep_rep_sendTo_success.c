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
}

int main(void) {
    ServiceSpec *serviceSpec = (ServiceSpec *)malloc(sizeof(ServiceSpec));
    if (serviceSpec != NULL) {
        serviceSpec->address = "127.0.0.1";
        serviceSpec->port = 1234;
        init(serviceSpec);
        void *ctx = zmq_ctx_new();
        void *req = zmq_socket(ctx, ZMQ_REQ);
        int result = zmq_connect(req, "tcp://127.0.0.1:1234");
        if (result == -1)
            puts("request connect failed");
        //  Send message from client to server
        int rc = zmq_send(req, "rose", 4, 0);
        if (rc == -1)
            puts("client send failed");
        puts("waitng for message");
        receive(handler);
        // send message from server to client
        result = sendTo(NULL,"Hello World",NULL);
        if (result == -1)
            puts("sendTo failed");

        //  Receive message at client side
        char buffer [12];
        rc = zmq_recv(req, buffer, 11, 0);
        if (rc == -1) {
            puts("receive failed");
            zmq_close(req);
            zmq_ctx_term(ctx);
        } else {
            buffer[rc] = '\0';
            printf("message received is %s\n",buffer);
            zmq_close(req);
            zmq_ctx_term(ctx);
            exit(EXIT_SUCCESS);
        }
    }
    exit(EXIT_FAILURE);
}
