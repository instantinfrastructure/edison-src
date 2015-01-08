
INCLUDE_DIRECTORIES(
  ../../lib/cJSON
  ../../lib/libiotkit-comm
  ../inc/
)

# set the source files list
SET(ZMQPUBSUB_CLIENT_RECEIVE_FAIL test_zmqpubsub_client_receive_fail.c ../../lib/plugins/libzmqpubsub/zmqpubsub-client.c ../../lib/cJSON/cJSON.c ../../lib/libiotkit-comm/iotkit-comm.c ../../lib/libiotkit-comm/iotkit-comm_mdns.c)
SET(ZMQPUBSUB_CLIENT_SOCKET_CONNECT_FAIL test_zmqpubsub_client_socket_connect_fail.c ../../lib/plugins/libzmqpubsub/zmqpubsub-client.c ../../lib/cJSON/cJSON.c ../../lib/libiotkit-comm/iotkit-comm.c ../../lib/libiotkit-comm/iotkit-comm_mdns.c)
SET(ZMQPUBSUB_CLIENT_SOCKET_CONNECT_SUCCESS test_zmqpubsub_client_socket_connect_success.c ../../lib/plugins/libzmqpubsub/zmqpubsub-client.c ../../lib/cJSON/cJSON.c ../../lib/libiotkit-comm/iotkit-comm.c ../../lib/libiotkit-comm/iotkit-comm_mdns.c)
SET(ZMQPUBSUB_CLIENT_SUBSCRIBE_FAIL test_zmqpubsub_client_subscribe_fail.c ../../lib/plugins/libzmqpubsub/zmqpubsub-client.c ../../lib/cJSON/cJSON.c ../../lib/libiotkit-comm/iotkit-comm.c ../../lib/libiotkit-comm/iotkit-comm_mdns.c)
SET(ZMQPUBSUB_CLIENT_SUBSCRIBE_SUCCESS test_zmqpubsub_client_subscribe_success.c ../../lib/plugins/libzmqpubsub/zmqpubsub-client.c ../../lib/cJSON/cJSON.c ../../lib/libiotkit-comm/iotkit-comm.c ../../lib/libiotkit-comm/iotkit-comm_mdns.c)
SET(ZMQPUBSUB_CLIENT_UNSUBSCRIBE_FAIL test_zmqpubsub_client_unsubscribe_fail.c ../../lib/plugins/libzmqpubsub/zmqpubsub-client.c ../../lib/cJSON/cJSON.c ../../lib/libiotkit-comm/iotkit-comm.c ../../lib/libiotkit-comm/iotkit-comm_mdns.c)
SET(ZMQPUBSUB_CLIENT_UNSUBSCRIBE_SUCCESS test_zmqpubsub_client_unsubscribe_success.c ../../lib/plugins/libzmqpubsub/zmqpubsub-client.c ../../lib/cJSON/cJSON.c ../../lib/libiotkit-comm/iotkit-comm.c ../../lib/libiotkit-comm/iotkit-comm_mdns.c)
SET(ZMQPUBSUB_SERVICE_PUBLISH_FAIL test_zmqpubsub_service_publish_fail.c ../../lib/plugins/libzmqpubsub/zmqpubsub-service.c ../../lib/cJSON/cJSON.c ../../lib/libiotkit-comm/iotkit-comm.c ../../lib/libiotkit-comm/iotkit-comm_mdns.c)
SET(ZMQPUBSUB_SERVICE_SOCKET_CONNECT_FAIL test_zmqpubsub_service_socket_connect_fail.c ../../lib/plugins/libzmqpubsub/zmqpubsub-service.c ../../lib/cJSON/cJSON.c ../../lib/libiotkit-comm/iotkit-comm.c ../../lib/libiotkit-comm/iotkit-comm_mdns.c)
SET(ZMQPUBSUB_SERVICE_SOCKET_CONNECT_SUCCESS test_zmqpubsub_service_socket_connect_success.c ../../lib/plugins/libzmqpubsub/zmqpubsub-service.c ../../lib/cJSON/cJSON.c ../../lib/libiotkit-comm/iotkit-comm.c ../../lib/libiotkit-comm/iotkit-comm_mdns.c)

# create the executables
ADD_EXECUTABLE(zmqpubsub_client_receive_fail ${ZMQPUBSUB_CLIENT_RECEIVE_FAIL})
ADD_EXECUTABLE(zmqpubsub_client_socket_connect_fail ${ZMQPUBSUB_CLIENT_SOCKET_CONNECT_FAIL})
ADD_EXECUTABLE(zmqpubsub_client_socket_connect_success ${ZMQPUBSUB_CLIENT_SOCKET_CONNECT_SUCCESS})
ADD_EXECUTABLE(zmqpubsub_client_subscribe_fail ${ZMQPUBSUB_CLIENT_SUBSCRIBE_FAIL})
ADD_EXECUTABLE(zmqpubsub_client_subscribe_success ${ZMQPUBSUB_CLIENT_SUBSCRIBE_SUCCESS})
ADD_EXECUTABLE(zmqpubsub_client_unsubscribe_fail ${ZMQPUBSUB_CLIENT_UNSUBSCRIBE_FAIL})
ADD_EXECUTABLE(zmqpubsub_client_unsubscribe_success ${ZMQPUBSUB_CLIENT_UNSUBSCRIBE_SUCCESS})
ADD_EXECUTABLE(zmqpubsub_service_publish_fail ${ZMQPUBSUB_SERVICE_PUBLISH_FAIL})
ADD_EXECUTABLE(zmqpubsub_service_socket_connect_fail ${ZMQPUBSUB_SERVICE_SOCKET_CONNECT_FAIL})
ADD_EXECUTABLE(zmqpubsub_service_socket_connect_success ${ZMQPUBSUB_SERVICE_SOCKET_CONNECT_SUCCESS})


SET_TARGET_PROPERTIES(zmqpubsub_client_receive_fail PROPERTIES LINKER_LANGUAGE C)
SET_TARGET_PROPERTIES(zmqpubsub_client_socket_connect_fail PROPERTIES LINKER_LANGUAGE C)
SET_TARGET_PROPERTIES(zmqpubsub_client_socket_connect_success PROPERTIES LINKER_LANGUAGE C)
SET_TARGET_PROPERTIES(zmqpubsub_client_subscribe_fail PROPERTIES LINKER_LANGUAGE C)
SET_TARGET_PROPERTIES(zmqpubsub_client_subscribe_success PROPERTIES LINKER_LANGUAGE C)
SET_TARGET_PROPERTIES(zmqpubsub_client_unsubscribe_fail PROPERTIES LINKER_LANGUAGE C)
SET_TARGET_PROPERTIES(zmqpubsub_client_unsubscribe_success PROPERTIES LINKER_LANGUAGE C)
SET_TARGET_PROPERTIES(zmqpubsub_service_publish_fail PROPERTIES LINKER_LANGUAGE C)
SET_TARGET_PROPERTIES(zmqpubsub_service_socket_connect_fail PROPERTIES LINKER_LANGUAGE C)
SET_TARGET_PROPERTIES(zmqpubsub_service_socket_connect_success PROPERTIES LINKER_LANGUAGE C)

# Link the executables
TARGET_LINK_LIBRARIES(zmqpubsub_client_receive_fail zmq dns_sd m dl gcov)
TARGET_LINK_LIBRARIES(zmqpubsub_client_socket_connect_fail zmq dns_sd m dl gcov)
TARGET_LINK_LIBRARIES(zmqpubsub_client_socket_connect_success zmq dns_sd m dl gcov)
TARGET_LINK_LIBRARIES(zmqpubsub_client_subscribe_fail zmq dns_sd m dl gcov)
TARGET_LINK_LIBRARIES(zmqpubsub_client_subscribe_success zmq dns_sd m dl gcov)
TARGET_LINK_LIBRARIES(zmqpubsub_client_unsubscribe_fail zmq dns_sd m dl gcov)
TARGET_LINK_LIBRARIES(zmqpubsub_client_unsubscribe_success zmq dns_sd m dl gcov)
TARGET_LINK_LIBRARIES(zmqpubsub_service_publish_fail zmq dns_sd m dl gcov)
TARGET_LINK_LIBRARIES(zmqpubsub_service_socket_connect_fail zmq dns_sd m dl gcov)
TARGET_LINK_LIBRARIES(zmqpubsub_service_socket_connect_success zmq dns_sd m dl gcov)

# Add the Test
ADD_TEST(zmqpubsub_client_receive_fail ${CTEST_BINARY_DIRECTORY}/zmqpubsub_client_receive_fail)
ADD_TEST(zmqpubsub_client_socket_connect_fail ${CTEST_BINARY_DIRECTORY}/zmqpubsub_client_socket_connect_fail)
ADD_TEST(zmqpubsub_client_socket_connect_success ${CTEST_BINARY_DIRECTORY}/zmqpubsub_client_socket_connect_success)
ADD_TEST(zmqpubsub_client_subscribe_fail ${CTEST_BINARY_DIRECTORY}/zmqpubsub_client_subscribe_fail)
ADD_TEST(zmqpubsub_client_subscribe_success ${CTEST_BINARY_DIRECTORY}/zmqpubsub_client_subscribe_success)
ADD_TEST(zmqpubsub_client_unsubscribe_fail ${CTEST_BINARY_DIRECTORY}/zmqpubsub_client_unsubscribe_fail)
ADD_TEST(zmqpubsub_client_unsubscribe_success ${CTEST_BINARY_DIRECTORY}/zmqpubsub_client_unsubscribe_success)
ADD_TEST(zmqpubsub_service_publish_fail ${CTEST_BINARY_DIRECTORY}/zmqpubsub_service_publish_fail)
ADD_TEST(zmqpubsub_service_socket_connect_fail ${CTEST_BINARY_DIRECTORY}/zmqpubsub_service_socket_connect_fail)
ADD_TEST(zmqpubsub_service_socket_connect_success ${CTEST_BINARY_DIRECTORY}/zmqpubsub_service_socket_connect_success)
