/*
 * ZMQ PUB/SUB plugin to enable subscribe feature through iotkit-comm API
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

/** @file zmqpubsub-client.h
    Header file. List of all functions in zmq subscribe client.
*/

#include <stdio.h>
#include <zmq.h>
#include <zmq_utils.h>
#include "../inc/zhelpers.h"

#ifndef DEBUG
    #define DEBUG 0
#endif

#include "iotkit-comm.h"

/** @defgroup zmqpubsubclient
*
*  @{
*/

char *interface = "client-interface"; /**< specifies the plugin interface json */

/** Structure holds the context and subscriber handler
*/
struct ZMQPubSubClient {
    void *context; /**< context handler */
    void *subscriber; /**< subscriber handler */
};

/** An Global ZMQPubSubClient Object.
*/
struct ZMQPubSubClient zmqContainer;

int init(void *ClientServiceQuery);
int send(char *message,Context context);
int subscribe(char *topic);
int unsubscribe(char *topic);
int receive(void (*subscribeClientHandler)(char *message, Context context));
int done();

/** @} */ // end of zmqpubsubclient
