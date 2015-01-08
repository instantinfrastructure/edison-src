
INCLUDE_DIRECTORIES(
  ../../lib/cJSON
  ../../lib/libiotkit-comm
  ../inc/
)

# set the source files list
SET(ZMQREQREP_REP_RECEIVE_FAIL test_zmqreqrep_rep_receive_fail.c ../../lib/plugins/libzmqreqrep/zmqreqrep-service.c ../../lib/cJSON/cJSON.c ../../lib/libiotkit-comm/iotkit-comm.c ../../lib/libiotkit-comm/iotkit-comm_mdns.c)
SET(ZMQREQREP_REP_RECEIVE_SUCCESS test_zmqreqrep_rep_receive_success.c ../../lib/plugins/libzmqreqrep/zmqreqrep-service.c ../../lib/cJSON/cJSON.c ../../lib/libiotkit-comm/iotkit-comm.c ../../lib/libiotkit-comm/iotkit-comm_mdns.c)
SET(ZMQREQREP_REP_SENDTO_FAIL test_zmqreqrep_rep_sendTo_fail.c ../../lib/plugins/libzmqreqrep/zmqreqrep-service.c ../../lib/cJSON/cJSON.c ../../lib/libiotkit-comm/iotkit-comm.c ../../lib/libiotkit-comm/iotkit-comm_mdns.c)
SET(ZMQREQREP_REP_SENDTO_SUCCESS test_zmqreqrep_rep_sendTo_success.c ../../lib/plugins/libzmqreqrep/zmqreqrep-service.c ../../lib/cJSON/cJSON.c ../../lib/libiotkit-comm/iotkit-comm.c ../../lib/libiotkit-comm/iotkit-comm_mdns.c)
SET(ZMQREQREP_REP_SOCKET_CONNECT_FAIL test_zmqreqrep_rep_socket_connect_fail.c ../../lib/plugins/libzmqreqrep/zmqreqrep-service.c ../../lib/cJSON/cJSON.c ../../lib/libiotkit-comm/iotkit-comm.c ../../lib/libiotkit-comm/iotkit-comm_mdns.c)
SET(ZMQREQREP_REP_SOCKET_CONNECT_SUCCESS test_zmqreqrep_rep_socket_connect_success.c ../../lib/plugins/libzmqreqrep/zmqreqrep-service.c ../../lib/cJSON/cJSON.c ../../lib/libiotkit-comm/iotkit-comm.c ../../lib/libiotkit-comm/iotkit-comm_mdns.c)
SET(ZMQREQREP_REQ_RECEIVE_FAIL test_zmqreqrep_req_receive_fail.c ../../lib/plugins/libzmqreqrep/zmqreqrep-client.c ../../lib/cJSON/cJSON.c ../../lib/libiotkit-comm/iotkit-comm.c ../../lib/libiotkit-comm/iotkit-comm_mdns.c)
SET(ZMQREQREP_REQ_SEND_FAIL test_zmqreqrep_req_send_fail.c ../../lib/plugins/libzmqreqrep/zmqreqrep-client.c ../../lib/cJSON/cJSON.c ../../lib/libiotkit-comm/iotkit-comm.c ../../lib/libiotkit-comm/iotkit-comm_mdns.c)
SET(ZMQREQREP_REQ_SEND_SUCCESS test_zmqreqrep_req_send_success.c ../../lib/plugins/libzmqreqrep/zmqreqrep-client.c ../../lib/cJSON/cJSON.c ../../lib/libiotkit-comm/iotkit-comm.c ../../lib/libiotkit-comm/iotkit-comm_mdns.c)
SET(ZMQREQREP_REQ_SOCKET_CONNECT_FAIL test_zmqreqrep_req_socket_connect_fail.c ../../lib/plugins/libzmqreqrep/zmqreqrep-client.c ../../lib/cJSON/cJSON.c ../../lib/libiotkit-comm/iotkit-comm.c ../../lib/libiotkit-comm/iotkit-comm_mdns.c)
SET(ZMQREQREP_REQ_SOCKET_CONNECT_SUCCESS test_zmqreqrep_req_socket_connect_success.c ../../lib/plugins/libzmqreqrep/zmqreqrep-client.c ../../lib/cJSON/cJSON.c ../../lib/libiotkit-comm/iotkit-comm.c ../../lib/libiotkit-comm/iotkit-comm_mdns.c)

# create the executables
ADD_EXECUTABLE(zmqreqrep_rep_receive_fail ${ZMQREQREP_REP_RECEIVE_FAIL})
ADD_EXECUTABLE(zmqreqrep_rep_receive_success ${ZMQREQREP_REP_RECEIVE_SUCCESS})
ADD_EXECUTABLE(zmqreqrep_rep_sendTo_fail ${ZMQREQREP_REP_SENDTO_FAIL})
ADD_EXECUTABLE(zmqreqrep_rep_sendTo_success ${ZMQREQREP_REP_SENDTO_SUCCESS})
ADD_EXECUTABLE(zmqreqrep_rep_socket_connect_fail ${ZMQREQREP_REP_SOCKET_CONNECT_FAIL})
ADD_EXECUTABLE(zmqreqrep_rep_socket_connect_success ${ZMQREQREP_REP_SOCKET_CONNECT_SUCCESS})
ADD_EXECUTABLE(zmqreqrep_req_receive_fail ${ZMQREQREP_REQ_RECEIVE_FAIL})
ADD_EXECUTABLE(zmqreqrep_req_send_fail ${ZMQREQREP_REQ_SEND_FAIL})
ADD_EXECUTABLE(zmqreqrep_req_send_success ${ZMQREQREP_REQ_SEND_SUCCESS})
ADD_EXECUTABLE(zmqreqrep_req_socket_connect_fail ${ZMQREQREP_REQ_SOCKET_CONNECT_FAIL})
ADD_EXECUTABLE(zmqreqrep_req_socket_connect_success ${ZMQREQREP_REQ_SOCKET_CONNECT_SUCCESS})


SET_TARGET_PROPERTIES(zmqreqrep_rep_receive_fail PROPERTIES LINKER_LANGUAGE C)
SET_TARGET_PROPERTIES(zmqreqrep_rep_receive_success PROPERTIES LINKER_LANGUAGE C)
SET_TARGET_PROPERTIES(zmqreqrep_rep_sendTo_fail PROPERTIES LINKER_LANGUAGE C)
SET_TARGET_PROPERTIES(zmqreqrep_rep_sendTo_success PROPERTIES LINKER_LANGUAGE C)
SET_TARGET_PROPERTIES(zmqreqrep_rep_socket_connect_fail PROPERTIES LINKER_LANGUAGE C)
SET_TARGET_PROPERTIES(zmqreqrep_rep_socket_connect_success PROPERTIES LINKER_LANGUAGE C)
SET_TARGET_PROPERTIES(zmqreqrep_req_receive_fail PROPERTIES LINKER_LANGUAGE C)
SET_TARGET_PROPERTIES(zmqreqrep_req_send_fail PROPERTIES LINKER_LANGUAGE C)
SET_TARGET_PROPERTIES(zmqreqrep_req_send_success PROPERTIES LINKER_LANGUAGE C)
SET_TARGET_PROPERTIES(zmqreqrep_req_socket_connect_fail PROPERTIES LINKER_LANGUAGE C)
SET_TARGET_PROPERTIES(zmqreqrep_req_socket_connect_success PROPERTIES LINKER_LANGUAGE C)


# Link the executables
TARGET_LINK_LIBRARIES(zmqreqrep_rep_receive_fail zmq dns_sd m dl gcov)
TARGET_LINK_LIBRARIES(zmqreqrep_rep_receive_success zmq dns_sd m dl gcov)
TARGET_LINK_LIBRARIES(zmqreqrep_rep_sendTo_fail zmq dns_sd m dl gcov)
TARGET_LINK_LIBRARIES(zmqreqrep_rep_sendTo_success zmq dns_sd m dl gcov)
TARGET_LINK_LIBRARIES(zmqreqrep_rep_socket_connect_fail zmq dns_sd m dl gcov)
TARGET_LINK_LIBRARIES(zmqreqrep_rep_socket_connect_success zmq dns_sd m dl gcov)
TARGET_LINK_LIBRARIES(zmqreqrep_req_receive_fail zmq dns_sd m dl gcov)
TARGET_LINK_LIBRARIES(zmqreqrep_req_send_fail zmq dns_sd m dl gcov)
TARGET_LINK_LIBRARIES(zmqreqrep_req_send_success zmq dns_sd m dl gcov)
TARGET_LINK_LIBRARIES(zmqreqrep_req_socket_connect_fail zmq dns_sd m dl gcov)
TARGET_LINK_LIBRARIES(zmqreqrep_req_socket_connect_success zmq dns_sd m dl gcov)

# Add the Test
ADD_TEST(zmqreqrep_rep_receive_fail ${CTEST_BINARY_DIRECTORY}/zmqreqrep_rep_receive_fail)
ADD_TEST(zmqreqrep_rep_receive_success ${CTEST_BINARY_DIRECTORY}/zmqreqrep_rep_receive_success)
ADD_TEST(zmqreqrep_rep_sendTo_fail ${CTEST_BINARY_DIRECTORY}/zmqreqrep_rep_sendTo_fail)
ADD_TEST(zmqreqrep_rep_sendTo_success ${CTEST_BINARY_DIRECTORY}/zmqreqrep_rep_sendTo_success)
ADD_TEST(zmqreqrep_rep_socket_connect_fail ${CTEST_BINARY_DIRECTORY}/zmqreqrep_rep_socket_connect_fail)
ADD_TEST(zmqreqrep_rep_socket_connect_success ${CTEST_BINARY_DIRECTORY}/zmqreqrep_rep_socket_connect_success)
ADD_TEST(zmqreqrep_req_receive_fail ${CTEST_BINARY_DIRECTORY}/zmqreqrep_req_receive_fail)
ADD_TEST(zmqreqrep_req_send_fail ${CTEST_BINARY_DIRECTORY}/zmqreqrep_req_send_fail)
ADD_TEST(zmqreqrep_req_send_success ${CTEST_BINARY_DIRECTORY}/zmqreqrep_req_send_success)
ADD_TEST(zmqreqrep_req_socket_connect_fail ${CTEST_BINARY_DIRECTORY}/zmqreqrep_req_socket_connect_fail)
ADD_TEST(zmqreqrep_req_socket_connect_success ${CTEST_BINARY_DIRECTORY}/zmqreqrep_req_socket_connect_success)
