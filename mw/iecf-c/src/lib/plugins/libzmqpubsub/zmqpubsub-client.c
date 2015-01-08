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

/** @file zmqpubsub-client.c
    Provides functions to subscribe on a topic, receive messages on a topic, unsubscribe from a topic.
*/

#include "zmqpubsub-client.h"

/** @defgroup zmqpubsubclient
*   This is ZMQ subscribe client.
*  @{
*/

/** Creates the context object and subscriber socket. With the help of the ServiceQuery parameter, the subscriber socket
establishes connection to the address and port to initiate communication.

* @param ClientServiceQuery an void pointer
* @return The result code
*/
int init(void *ClientServiceQuery) {
    #if DEBUG
        printf("In createClient\n");
    #endif
    ServiceQuery *serviceQuery = (ServiceQuery *)ClientServiceQuery;
    zmqContainer.context = zmq_ctx_new();
    #if DEBUG
        printf("context initialised\n");
    #endif
    char addr[128];
    sprintf(addr, "tcp://%s:%d\n", serviceQuery->address, serviceQuery->port);
    #if DEBUG
        printf("going to connect %s\n",addr);
    #endif
    zmqContainer.subscriber = zmq_socket(zmqContainer.context,ZMQ_SUB);
    int rc = zmq_connect(zmqContainer.subscriber,addr);
    #if DEBUG
        printf("subscriber connected\n");
    #endif
    return rc;
}

int send(char *message,Context context) {
    #if DEBUG
        printf("\ninside sending message: %s\n",message);
    #endif
    return -1;
}

/** Subscribing to a topic. The client can subscribe to a topic in which he is interested to receive the messages.
* @param topic client is interested in
* @return The result code
*/
int subscribe(char *topic) {
    #if DEBUG
        printf("\nsubscribing for topic: %s\n",topic);
    #endif
    //  Subscribe only for the topic
    int rc = zmq_setsockopt (zmqContainer.subscriber, ZMQ_SUBSCRIBE, topic, strlen(topic));
    return rc;
}

/** Receive the message. The parameter in this function is used as a callback mechanism to pass the
received message.
* @param subscribeClientHandler a callback handler
* @return The result code
*/
int receive(void (*subscribeClientHandler)(char *message, Context context)) {
    #if DEBUG
        printf("In receive Waiting for the message\n");
    #endif
    //  Read message contents
    char *contents = s_recv (zmqContainer.subscriber);
    #if DEBUG
        printf ("Message Received: %s\n", contents);
    #endif
    Context context;
    context.name = "event";
    context.value = "message";
    #if DEBUG
        printf ("calling handler\n");
    #endif
    subscribeClientHandler(contents,context);
    #if DEBUG
        printf ("\nfree the contents\n");
    #endif
    free (contents);
    return 0;
}

/** Unsubscribing to a topic. The client can unsubscribe, to stop receiving the messages on that topic.
* @param topic client wants to unsubscribe from
* @return The result code
*/
int unsubscribe(char *topic) {
    #if DEBUG
        printf("\nunsubscribing the topic: %s\n",topic);
    #endif
    int rc = zmq_setsockopt (zmqContainer.subscriber, ZMQ_UNSUBSCRIBE, topic, strlen(topic));
    return rc;
}

/** Cleanup function. This function close the subscriber socket and destroy the
context object.
* @return The result code
*/
int done() {
    if (zmqContainer.subscriber != NULL) {
        zmq_close(zmqContainer.subscriber);
        zmqContainer.subscriber = NULL;
        #if DEBUG
            printf("subscriber freed\n");
        #endif
    }
    if (zmqContainer.context != NULL) {
        zmq_ctx_destroy(zmqContainer.context);
        zmqContainer.context = NULL;
        #if DEBUG
            printf("context freed\n");
        #endif
    }
    #if DEBUG
        printf("\nclosed\n");
    #endif
    return 0;
}

/** @} */ // end of zmqpubsubclient
