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
* @file mqtt-client.c
* @brief Implementation of MQTT Client plugin for iotkit-comm API.
*
* Provides features to connect to an MQTT Broker and subscribe to a topic.
*/

#include "mqtt-client.h"

int messageArrived(void *ctx, char *topicName, int topicLen, MQTTClient_message *message) {
    char *payloadmsg;
    Context context;

    #if DEBUG
        char* payloadptr;
        int i;
        printf("Message arrived\n");
        printf("topic: %s\n", topicName);
        printf("message:");

        payloadptr = message->payload;
        for(i = 0; i < message->payloadlen; i++) {
            putchar(*payloadptr++);
        }
        putchar('\n');
    #endif

    payloadmsg = (char *)malloc(message->payloadlen+1);
    if (payloadmsg != NULL) {
        strncpy(payloadmsg, message->payload, message->payloadlen);
        payloadmsg[message->payloadlen] = '\0';
    }
    context.name = "topic";
    context.value = strdup(topicName);
    if (msgArrhandler != NULL) {
        msgArrhandler(payloadmsg, context);
    } else {
        printf("error: Receive Handler not set\n");
    }

    free(payloadmsg);
    free(context.value);
    return true;
}

void connectionLost(void *context, char *cause) {
    ServiceQuery *serviceQuery = (ServiceQuery *)context;
    int rc;

    printf("Connection lost due to :%s\n", cause);
    printf("Reconnecting...\n");

    conn_opts.cleansession = 1;
    conn_opts.keepAliveInterval = 20;
    conn_opts.retryInterval = 1000;

    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to start connect, return code %d\n", rc);
        exit(1);
    }
}

/**
 * @name Publish a message
 * @brief Used to send message to a broker.
 * @param[in] message to be published
 * @param[in] context w.r.t topic the message required to be published
 * @return boolean, specifies whether the message is successfully published or not
 */
int send(char *message, Context context) {
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;

    int rc = 0;
    char *topic;

    if (context.name != NULL && context.value != NULL && strcmp(context.name, "topic") == 0) {
        topic = context.value;
    } else {
        printf("Topic not available in the send command");
        return MQTTCLIENT_NULL_PARAMETER;
    }

    pubmsg.payload = message;
    pubmsg.payloadlen = strlen(message);
    pubmsg.qos = QOS;
    pubmsg.retained = 0;

    MQTTClient_publishMessage(client, topic, &pubmsg, &token);

    #if DEBUG
        printf("Waiting for up to %d seconds for publication of %s\n"
        "on topic %s for client with ClientID: %s\n",
        (int)(TIMEOUT/1000), message, topic, clientID);
    #endif

    rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
    #if DEBUG
        printf("Message with delivery token %d delivered\n", token);
    #endif

    return rc;
}

/**
 * @name Subscribes to a topic
 * @brief Subscribes to a topic with an MQTT broker.
 * @param[in] topic which needs to be subscribed to
 * @return boolean, which specifies whether successfully subscribed or not
 */
int subscribe(char *topic) {
    int rc = 0;

    if ((rc = MQTTClient_subscribe(client, topic, QOS)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to subscribe, return code %d\n", rc);
        exit(-1);
    }

    return rc;
}

/**
 * @name Disconnect and destroy the MQTT client
 * @brief Used to close the connections for cleanup activities.
 * @return boolean, which specifies whether the connection is disconnected or not
 */
int done() {
    int rc = 0;

    if ((rc = MQTTClient_disconnect(client, 10000)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to start disconnect, return code %d\n", rc);
        exit(-1);
    }

    MQTTClient_destroy(&client);

    return rc;
}

/**
 * @name Unsubscribe a topic
 * @brief Discontinues the subscription to a topic.
 * @param[in] topic which has been previously subscribed to
 */
int unsubscribe(char *topic) {
    #if DEBUG
        printf("Invoked MQTT: unsubscribe()\n");
    #endif

    int rc = 0;

    if ((rc = MQTTClient_unsubscribe(client, topic)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to unsubscribe, return code %d\n", rc);
        exit(-1);
    }

    return rc;
}

/**
 * @name Subscribe to a topic
 * @brief Registers the client's callback to be invoked on receiving a message from MQTT broker.
 * @param handler to be registered as a callback
 */
int receive(void (*handler) (char *topic, Context context)) {
    #if DEBUG
        printf("Invoked MQTT: setReceivedMessageHandler()\n");
    #endif

    msgArrhandler = handler;

    return 1;
}

/**
 * @name Create and initialize the MQTT client
 * @brief initializes the plugin.
 * @param[in] servQuery is the service query being queried for
 * @return boolean, which specifies whether the connection is successfully established or not
 *
 * Establishes the connection with an MQTT broker.
 */
int init(void *servQuery) {
    ServiceQuery *serviceQuery = (ServiceQuery *) servQuery;
    int rc = 0;
    char uri[256];

    if (isPresentPropertyInCommParams(serviceQuery, "ssl") == true && \
        strcasecmp(getValueInCommParams(serviceQuery, "ssl"), "true") == 0) {
        sprintf(uri, "ssl://%s:%d", serviceQuery->address, serviceQuery->port);

        conn_opts.ssl = &sslopts;

        if (isPresentPropertyInCommParams(serviceQuery, "keyStore")) {
            conn_opts.ssl->keyStore = getValueInCommParams(serviceQuery, "keyStore");
        }
        if (isPresentPropertyInCommParams(serviceQuery, "privateKey")) {
            conn_opts.ssl->privateKey = getValueInCommParams(serviceQuery, "privateKey");
        }
        if (isPresentPropertyInCommParams(serviceQuery, "trustStore")) {
            conn_opts.ssl->trustStore = getValueInCommParams(serviceQuery, "trustStore");
        }

        conn_opts.ssl->enableServerCertAuth = 0;
    } else {
        if (serviceQuery->address != NULL) {
            sprintf(uri, "tcp://%s:%d", serviceQuery->address, serviceQuery->port);
        } else {
            sprintf(uri, "tcp://localhost:%d", serviceQuery->port);
        }
    }

    // Default settings:
    char clientID[256];
    sprintf(clientID, "%s_%d", CLIENTID, getpid());

    MQTTClient_create(&client, uri, clientID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    MQTTClient_setCallbacks(client, serviceQuery, connectionLost, messageArrived, NULL);

    conn_opts.cleansession = 0;
    conn_opts.keepAliveInterval = 20;
    conn_opts.retryInterval = 0;

    if (isPresentPropertyInCommParams(serviceQuery, "username") == true && \
        isPresentPropertyInCommParams(serviceQuery, "password") == true) {
        conn_opts.username = getValueInCommParams(serviceQuery, "username");
        conn_opts.password = getValueInCommParams(serviceQuery, "password");
    }

    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to start connect, return code %d\n", rc);
        exit(1);
    }

    return rc;
}
