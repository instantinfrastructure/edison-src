/*
 * Utility functions for parsing JSON object
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
* @file util.h
* @brief Helper class for JSON Parsing.
*
* Utility class for parsing JSON Objects.
*/

#include <stdbool.h>
#include "cJSON.h"

/** Check whether JSON item is a string. */
static inline bool isJsonString(cJSON *item) {
    if (item == NULL || item->type != cJSON_String) {
        return false;
    }
    return true;
}

/** Check whether JSON item is a number. */
static inline bool isJsonNumber(cJSON *item) {
    if (item == NULL || item->type != cJSON_Number) {
        return false;
    }
    return true;
}

/** Check whether JSON item is an object. */
static inline bool isJsonObject(cJSON *item) {
    if (item == NULL || item->type != cJSON_Object) {
        return false;
    }
    return true;
}

/** Check whether JSON item is an array. */
static inline bool isJsonArray(cJSON *item) {
    if (item == NULL || item->type != cJSON_Array) {
        return false;
    }
    return true;
}
