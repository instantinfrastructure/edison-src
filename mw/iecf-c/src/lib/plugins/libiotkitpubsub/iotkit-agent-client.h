/*
 * IoTKit Async client plugin to enable subscribe feature through iotkit-comm API
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
* @file iotkit-agent-client.h
* @brief Headers of iotkit Async Client plugin for iotkit-comm API.
*
* Provides features to connect to an MQTT Broker and subscribe to a topic.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <MQTTAsync.h>

#include "iotkit-comm.h"

#define CLIENTID    "IoTClient"
#define QOS         1
#define TIMEOUT     10000L

#ifndef DEBUG
    #define DEBUG 0
#endif

volatile int toStop = 0;
volatile int finished = 0;
volatile int subscribed = 0;
volatile int connected = 0;
volatile int quietMode = 0;
volatile int sent = 0;
volatile int delivery = 0;

int clientInstanceNumber = 0;

char *interface = "iotkit-agent-client-interface"; // specifies the plugin interface json

void *handle = NULL;
char *err = NULL;

MQTTAsync client;
MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;

void (*msgArrhandler) (char *topic, Context context) = NULL;

void registerSensor(char *sensorname, char *type);
int init(void *serviceDesc);
int send(char *message, Context context);
int subscribe();
int unsubscribe(char *topic);
int receive(void (*) (char *topic, Context context));
int done();
