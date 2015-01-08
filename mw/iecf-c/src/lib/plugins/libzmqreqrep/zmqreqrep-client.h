/*
 * ZMQ REQ/REP plugin to enable request feature through iotkit-comm API
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

/** @file zmqreqrep-client.h
    Header file. Lists all those functions in implementing ZMQ requester.
*/

#include <stdio.h>
#include <zmq.h>
#include <zmq_utils.h>

#include "iotkit-comm.h"

#ifndef DEBUG
    #define DEBUG 0
#endif

/** @defgroup zmqreqrepclient
*
*  @{
*/

char *interface = "client-interface"; /**< specifies the plugin interface json */

/** Structure holds the context and requester handler
*/
struct ZMQReqRepClient {
    void *context; /**< context handler */
    void *requester; /**< requester handler */
};

/** An Global ZMQReqRepClient Object.
*/
struct ZMQReqRepClient zmqContainer;

int init(void *requestClientQuery);
int send(char *message, Context);
int subscribe(char *topic);
int unsubscribe(char *topic);
int receive(void (*requestClientHandler)(char *message, Context context));
int done();
/** @} */ // end of zmqreqrepclient
