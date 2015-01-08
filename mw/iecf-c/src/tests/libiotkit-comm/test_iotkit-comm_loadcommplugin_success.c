/*
 * iotkit-comm API
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

/** @file test_iotkit-comm_loadcommplugin_success.c

*/

#include <stdio.h>
#include <zmq.h>
#include <zmq_utils.h>
#include "../../lib/libiotkit-comm/iotkit-comm.h"

int main(void) {
    CommHandle *plugin = loadCommPlugin("/usr/lib/libzmqpubsub-client.so");
    if (plugin) {
        puts("Success: Plugin Loaded");
        exit(EXIT_SUCCESS);
    } else {
        puts("Failed: Plugin Load");
        exit(EXIT_FAILURE);
    }
}
