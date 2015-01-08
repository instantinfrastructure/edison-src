/*
 * MDNS through iotkit-comm API
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

/** @file test_mdns_parse_servicedesc_fail.c

*/

#include <stdio.h>
#include <zmq.h>
#include <zmq_utils.h>
#include "../../lib/libiotkit-comm/iotkit-comm.h"

int main(void) {
    char *filepath = "./invalidtemperatureServiceMQTT.json";
    if (parseServiceSpec(filepath) != NULL) {
        puts("Success: Parse Service Specification");
        exit(EXIT_FAILURE);
    } else {
        puts("Failure: Parse Service Specification");
        exit(EXIT_SUCCESS);
    }
}
