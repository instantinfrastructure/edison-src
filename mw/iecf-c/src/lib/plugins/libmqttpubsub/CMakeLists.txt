
MESSAGE("Building the mqtt plugin")

INCLUDE_DIRECTORIES(
    ../../libiotkit-comm
    ../inc/
)

# set the shared library name
SET(MQTT_CLIENT_NAME "mqttpubsub-client")

# create the shared library
ADD_LIBRARY(${MQTT_CLIENT_NAME} SHARED mqtt-client.c mqtt-client.h)

SET_TARGET_PROPERTIES(${MQTT_CLIENT_NAME} PROPERTIES LINKER_LANGUAGE C)

# Link the shared library
TARGET_LINK_LIBRARIES(${MQTT_CLIENT_NAME} paho-mqtt3cs)
