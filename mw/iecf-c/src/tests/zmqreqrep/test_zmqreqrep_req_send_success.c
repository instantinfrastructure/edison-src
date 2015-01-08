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

/** @file test_zmqreqrep_req_send_fail.c

This file tests whether ZMQ Requester socket fails while sending message.
*/

#include <stdio.h>
#include <assert.h>
#include <zmq.h>
#include <zmq_utils.h>
#include "../../lib/libiotkit-comm/iotkit-comm.h"

void handler(char *message,Context context) {
    printf("Received message: %s\n",message);
}

int main(void) {
    ServiceQuery *serviceQuery = (ServiceQuery *)malloc(sizeof(ServiceQuery));
    if (serviceQuery != NULL) {
        serviceQuery->address = "127.0.0.1";
        serviceQuery->port = 5560;
        int result = init(serviceQuery);
        if (result == -1)
            puts("Requester init failed");
        void *ctx = zmq_ctx_new();
        assert (ctx);
        void *pub = zmq_socket(ctx, ZMQ_REP);
        assert (pub);
        int rc = zmq_bind(pub, "tcp://127.0.0.1:5560");
        assert (rc == 0);
        result = send("apple",NULL);
        if (result == -1) {
            puts("send failed");
            rc = zmq_close(pub);
            assert(rc == 0);
            rc = zmq_ctx_term(ctx);
            assert(rc == 0);
            free(serviceQuery);
        } else {
            puts("Requester Sent Message Successfully");
            rc = zmq_close(pub);
            assert (rc == 0);
            rc = zmq_ctx_term(ctx);
            assert(rc == 0);
            free(serviceQuery);
            exit(EXIT_SUCCESS);
        }
    }
    exit(EXIT_FAILURE);
}
