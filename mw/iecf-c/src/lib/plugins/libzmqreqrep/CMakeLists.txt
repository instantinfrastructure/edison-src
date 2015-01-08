
MESSAGE("Building the zmq reqrep plugin")

INCLUDE_DIRECTORIES(
    ../../libiotkit-comm
    ../inc/
)

# set the shared library name
SET(ZMQ_REQREP_CLIENT_NAME "zmqreqrep-client")
SET(ZMQ_REQREP_SERVICE_NAME "zmqreqrep-service")

# create the shared library
ADD_LIBRARY(${ZMQ_REQREP_CLIENT_NAME} SHARED zmqreqrep-client.c zmqreqrep-client.h)
ADD_LIBRARY(${ZMQ_REQREP_SERVICE_NAME} SHARED zmqreqrep-service.c zmqreqrep-service.h)

SET_TARGET_PROPERTIES(${ZMQ_REQREP_CLIENT_NAME} PROPERTIES LINKER_LANGUAGE C)
SET_TARGET_PROPERTIES(${ZMQ_REQREP_SERVICE_NAME} PROPERTIES LINKER_LANGUAGE C)

# Link the shared library
TARGET_LINK_LIBRARIES(${ZMQ_REQREP_CLIENT_NAME} zmq)
TARGET_LINK_LIBRARIES(${ZMQ_REQREP_SERVICE_NAME} zmq)
