
MESSAGE("Building the zmq pubsub plugin")

INCLUDE_DIRECTORIES(
    ../../libiotkit-comm
    ../inc/
)

# set the shared library name
SET(ZMQ_PUBSUB_CLIENT_NAME "zmqpubsub-client")
SET(ZMQ_PUBSUB_SERVICE_NAME "zmqpubsub-service")

# create the shared library
ADD_LIBRARY(${ZMQ_PUBSUB_CLIENT_NAME} SHARED zmqpubsub-client.c zmqpubsub-client.h)
ADD_LIBRARY(${ZMQ_PUBSUB_SERVICE_NAME} SHARED zmqpubsub-service.c zmqpubsub-service.h)

SET_TARGET_PROPERTIES(${ZMQ_PUBSUB_CLIENT_NAME} PROPERTIES LINKER_LANGUAGE C)
SET_TARGET_PROPERTIES(${ZMQ_PUBSUB_SERVICE_NAME} PROPERTIES LINKER_LANGUAGE C)

# Link the shared library
TARGET_LINK_LIBRARIES(${ZMQ_PUBSUB_CLIENT_NAME} zmq)
TARGET_LINK_LIBRARIES(${ZMQ_PUBSUB_SERVICE_NAME} zmq)
