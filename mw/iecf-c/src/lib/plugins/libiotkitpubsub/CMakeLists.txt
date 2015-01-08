
MESSAGE("Building the mqtt plugin")

INCLUDE_DIRECTORIES(
    ../../libiotkit-comm
    ../inc/
)

# set the shared library name
SET(IOTKIT_AGENT_CLIENT_NAME "iotkit-agent-client")

# create the shared library
ADD_LIBRARY(${IOTKIT_AGENT_CLIENT_NAME} SHARED iotkit-agent-client.c iotkit-agent-client.h)

SET_TARGET_PROPERTIES(${IOTKIT_AGENT_CLIENT_NAME} PROPERTIES LINKER_LANGUAGE C)

# Link the shared library
TARGET_LINK_LIBRARIES(${IOTKIT_AGENT_CLIENT_NAME} paho-mqtt3as)
