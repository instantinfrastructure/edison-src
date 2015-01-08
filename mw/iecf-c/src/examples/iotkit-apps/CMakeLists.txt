
INCLUDE_DIRECTORIES(
    ../../lib/cJSON
    ../../lib/libiotkit-comm
)

# set the executable names
SET(IOTKIT_CLIENT_NAME "iotkitclient")
SET(IOTKIT_SERVICE_NAME "iotkitservice")

SET(CLIENT_LIST iotkit-subscriber.c ../../lib/cJSON/cJSON.c ../../lib/libiotkit-comm/iotkit-comm.c ../../lib/libiotkit-comm/iotkit-comm_mdns.c)
SET(SERVICE_LIST iotkit-publisher.c ../../lib/cJSON/cJSON.c ../../lib/libiotkit-comm/iotkit-comm.c ../../lib/libiotkit-comm/iotkit-comm_mdns.c)

ADD_EXECUTABLE(${IOTKIT_CLIENT_NAME} ${CLIENT_LIST})
ADD_EXECUTABLE(${IOTKIT_SERVICE_NAME} ${SERVICE_LIST})

SET_TARGET_PROPERTIES(${IOTKIT_CLIENT_NAME} PROPERTIES LINKER_LANGUAGE C)
SET_TARGET_PROPERTIES(${IOTKIT_SERVICE_NAME} PROPERTIES LINKER_LANGUAGE C)

# Link the shared library
TARGET_LINK_LIBRARIES(${IOTKIT_CLIENT_NAME} paho-mqtt3as pthread dns_sd m dl)
TARGET_LINK_LIBRARIES(${IOTKIT_SERVICE_NAME} paho-mqtt3as pthread dns_sd m dl)
