/*
 * Distributed Thermostat sample program through iotkit-comm API
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

/** @file thermostat.c

    Sample client program of thermostat controller based on ZMQ Pub/Sub
*/

#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include "iotkit-comm.h"
#include "util.h"

// maybe better to use a simple moving average, but then again,
// this is just a demo of how to use iotkit-comm
int cumulativeMovingAverage = 0;

// no. of temperature samples received from all sensors until now 	30
int sampleCount = 0;

// service instance to publish the latest mean temperature
int (**mypublisher)(char *,Context context);

/** Callback function. To to be invoked when it receives any messages for the subscribed topic
* @param message the message received from service/publisher
* @param context a context object
 */
void clientMessageCallback(char *message, Context context) {

    // remove the topic from the message content (the temperature itself)
    char *temperature = strstr(message,":");
    if (temperature != NULL) {
            temperature++;
    fprintf(stdout,"Received sample temperature %s\n",temperature);
    // compute the mean of the temperatures as they arrive
    double value = atof(temperature);
    sampleCount++;
    cumulativeMovingAverage = (value + sampleCount * cumulativeMovingAverage)/(sampleCount + 1);
    char addr[256];
    sprintf(addr, "%d", cumulativeMovingAverage);
    fprintf(stdout,"New average ambient temperature (cumulative) %s:\n",addr);
    char mean[256];
    sprintf(mean, "mean_temp: %s", addr);
    // the master (thermostat) publishes the average temperature so others
    // can subscribe to it.
    if (mypublisher != NULL)
        (*mypublisher)(mean,context);
    }
}

/** Callback function. Once the service is discovered this callback function will be invoked
* @param servQuery the service query object
* @param error_code the error code
* @param commHandle the communication handle used to invoke the interfaces
 */
void subCallback(ServiceQuery *servQuery, int32_t error_code, CommHandle *commHandle) {

    int (**subscribe)(char *);
    int (**receive)(void (*)(char *, Context));
    ServiceSpec *serviceSpec = (ServiceSpec *) parseServiceSpec("./serviceSpecs/thermostat-spec.json");
    if (serviceSpec) {
        advertiseService(serviceSpec);
        CommHandle *serviceHandle = createService(serviceSpec);
        mypublisher = commInterfacesLookup(serviceHandle, "publish");
    }
    if (commHandle != NULL) {
        subscribe = commInterfacesLookup(commHandle, "subscribe");
        receive = commInterfacesLookup(commHandle, "receive");
        if (subscribe != NULL && receive != NULL) {
            while (1) {  // Infinite Event Loop
                (*subscribe)("mytemp");
                (*receive)(clientMessageCallback);
                sleep(2);
            }
        } else {
            fprintf(stderr, "Interface lookup failed\n");
        }
    } else {
        fprintf(stderr, "Comm Handle is NULL\n");
    }
}

/** The starting point. Starts browsing for the given Service name
 */
int main(void) {

    puts("Thermostat reading sensor data & publishing it to Dashboard");
    ServiceQuery *servQuery = (ServiceQuery *) parseServiceQuery("./serviceQueries/temperature-sensor-query.json");
    if (servQuery) {
        discoverServicesBlocking(servQuery, subCallback);
    }
    return 0;
}
