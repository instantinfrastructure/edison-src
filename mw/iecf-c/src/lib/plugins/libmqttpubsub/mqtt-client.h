/*
 * MQTT client plugin to enable subscribe feature through iotkit-comm API
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

/**
* @file mqtt-client.h
* @brief Headers of MQTT Async Client plugin for iotkit-comm API.
*
* Provides features to connect to an MQTT Broker and subscribe to a topic.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <MQTTClient.h>

#include "iotkit-comm.h"

#define CLIENTID    "MQTTClient"
#define QOS         1
#define TIMEOUT     10000L

#ifndef DEBUG
    #define DEBUG 0
#endif

void *handle = NULL;
char *err = NULL;

MQTTClient client;
MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
MQTTClient_SSLOptions sslopts = MQTTClient_SSLOptions_initializer;

char *interface = "client-interface"; // specifies the plugin interface json

int init(void *serviceDesc);
int send(char *message, Context context);
int subscribe(char *topic);
int unsubscribe(char *topic);
int receive(void (*) (char *topic, Context context));
int done();

void (*msgArrhandler) (char *topic, Context context) = NULL;
