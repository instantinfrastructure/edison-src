/*
 * iotkit-comm 'C' Library to load plugins on-demand
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
* @file iotkit-comm.h
* @brief Header file of iotkit-comm Library.
*
* Data Structure of Service Specification, Service Query, Communication Handle and Context.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdbool.h>
#include <dlfcn.h>
#include <pwd.h>

#ifndef DEBUG
    #define DEBUG 0
#endif

#ifndef LIB_CONFIG_DIRECTORY
    #define LIB_CONFIG_DIRECTORY "/etc/iotkit-comm/"
#endif

#ifndef LIB_CONFIG_FILENAME
    #define LIB_CONFIG_FILENAME "config.json"
#endif

#ifndef USER_CONFIG_FILENAME
    #define USER_CONFIG_FILENAME ".iotkit-comm_config.json"
#endif

#ifndef LIB_PLUGINS_DIRECTORY
    #define LIB_PLUGINS_DIRECTORY "/usr/lib/"
#endif

/** System level Configuration data read from the config JSON.
*/
typedef struct _ConfigFileData {
    char *pluginInterfaceDir; // plugin interface directory
    char *pluginDir; // plugin directory
    char *clientFileSuffix; // client plugin suffix
    char *serverFileSuffix; // service plugin suffix
} ConfigFileData;

/** Context to be passed around the callback methods as a name-value pair.
*/
typedef struct _Context {
    char *name;
    char *value;
} Context;

typedef struct _Interfaces {
    char *iname; // interface name
    void *iptr; // interface or function address
} Interfaces;

/** Handle to the communication plugin.
*/
typedef struct {
    char **interface; // specifies the filename for plugin-interface json file

    int (*init)(void *); // initializes the plugin

    Interfaces **interfaces;
    int interfacesCount;
    void *handle; // handle to the library
} CommHandle;

/** Property stored as key-value pair.
*/
typedef struct _Prop {
    char *key;
    char *value;
} Property;

/** Service specification and query.
 */
typedef struct _ServiceSpec {
    enum { ADDED, REMOVED, REGISTERED, IN_USE, UNKNOWN } status; // current status of the service/client
    char *service_name; // name of the service
    struct {
        char *name; // serive type name
        char *protocol; // service protocol
    } type;
    char *address; // address where service is available
    int port; // port at which service is running
    int commParamsCount; // count of comm params
    Property **comm_params; // list of comm params
    int numProperties; // count of properties
    Property **properties; // list of properties
    struct {
        char *locally;
        char *cloud;
    } advertise;
} ServiceSpec,ServiceQuery;

ConfigFileData g_configData;

/** list of function signatures specified in the interface JSON
*/
char **g_funcSignatures;
int g_funcEntries;

CommHandle *createClient(ServiceQuery *);
CommHandle *createService(ServiceSpec *);
void *commInterfacesLookup(CommHandle *commHandle, char *funcname);
void cleanUp(CommHandle *);
bool fileExists(char *absPath);
