
INCLUDE_DIRECTORIES(
    ../../lib/cJSON
    ../../lib/libiotkit-comm
)

# set the executable names
SET(ZMQ_SUB_CLIENT zmqsubclient)
SET(ZMQ_PUB_SERVICE zmqpubservice)
SET(ZMQ_REQ_CLIENT zmqreqclient)
SET(ZMQ_REP_SERVICE zmqrepservice)

# set the source files list
SET(SUB_CLIENT_LIST sample_zmqpubsub-client.c ../../lib/cJSON/cJSON.c ../../lib/libiotkit-comm/iotkit-comm.c ../../lib/libiotkit-comm/iotkit-comm_mdns.c)
SET(PUB_SERVICE_LIST sample_zmqpubsub-service.c ../../lib/cJSON/cJSON.c ../../lib/libiotkit-comm/iotkit-comm.c ../../lib/libiotkit-comm/iotkit-comm_mdns.c)
SET(REQ_CLIENT_LIST sample_zmqreqrep-client.c ../../lib/cJSON/cJSON.c ../../lib/libiotkit-comm/iotkit-comm.c ../../lib/libiotkit-comm/iotkit-comm_mdns.c)
SET(REP_SERVICE_LIST sample_zmqreqrep-service.c ../../lib/cJSON/cJSON.c ../../lib/libiotkit-comm/iotkit-comm.c ../../lib/libiotkit-comm/iotkit-comm_mdns.c)

# create the executables
ADD_EXECUTABLE(${ZMQ_SUB_CLIENT} ${SUB_CLIENT_LIST})
ADD_EXECUTABLE(${ZMQ_PUB_SERVICE} ${PUB_SERVICE_LIST})
ADD_EXECUTABLE(${ZMQ_REQ_CLIENT} ${REQ_CLIENT_LIST})
ADD_EXECUTABLE(${ZMQ_REP_SERVICE} ${REP_SERVICE_LIST})

SET_TARGET_PROPERTIES(${ZMQ_SUB_CLIENT} PROPERTIES LINKER_LANGUAGE C)
SET_TARGET_PROPERTIES(${ZMQ_PUB_SERVICE} PROPERTIES LINKER_LANGUAGE C)
SET_TARGET_PROPERTIES(${ZMQ_REQ_CLIENT} PROPERTIES LINKER_LANGUAGE C)
SET_TARGET_PROPERTIES(${ZMQ_REP_SERVICE} PROPERTIES LINKER_LANGUAGE C)

# Link the executables
TARGET_LINK_LIBRARIES(${ZMQ_SUB_CLIENT} zmq pthread dns_sd m dl)
TARGET_LINK_LIBRARIES(${ZMQ_PUB_SERVICE} zmq pthread dns_sd m dl)
TARGET_LINK_LIBRARIES(${ZMQ_REQ_CLIENT} zmq pthread dns_sd m dl)
TARGET_LINK_LIBRARIES(${ZMQ_REP_SERVICE} zmq pthread dns_sd m dl)
