
MESSAGE("Building the IoT communication Library")

INCLUDE_DIRECTORIES(
    ../cJSON
)

# Set the shared library name
SET(IOTKIT_COMM_LIBRARY_NAME "iotkit-comm")

# Create the shared library
ADD_LIBRARY(${IOTKIT_COMM_LIBRARY_NAME} SHARED iotkit-comm.c iotkit-comm_mdns.c ../cJSON/cJSON.c)

SET_TARGET_PROPERTIES(${IOTKIT_COMM_LIBRARY_NAME} PROPERTIES LINKER_LANGUAGE C)

# Link the shared library
TARGET_LINK_LIBRARIES(${IOTKIT_COMM_LIBRARY_NAME} paho-mqtt3as)
