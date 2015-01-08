
INCLUDE_DIRECTORIES(
  ../../lib/cJSON
  ../../lib/libiotkit-comm
  ../inc/
)

# set the source files list
SET(MQTTPUBSUB_CLIENT_PUBLISH_SUCCESS test_mqttpubsub_client_publish_success.c ../../lib/plugins/libmqttpubsub/mqtt-client.c ../../lib/cJSON/cJSON.c ../../lib/libiotkit-comm/iotkit-comm.c ../../lib/libiotkit-comm/iotkit-comm_mdns.c)
SET(MQTTPUBSUB_CLIENT_PUBLISH_FAIL test_mqttpubsub_client_publish_fail.c ../../lib/plugins/libmqttpubsub/mqtt-client.c ../../lib/cJSON/cJSON.c ../../lib/libiotkit-comm/iotkit-comm.c ../../lib/libiotkit-comm/iotkit-comm_mdns.c)
SET(MQTTPUBSUB_CLIENT_CONNECT_SUCCESS test_mqttpubsub_client_connect_success.c ../../lib/plugins/libmqttpubsub/mqtt-client.c ../../lib/cJSON/cJSON.c ../../lib/libiotkit-comm/iotkit-comm.c ../../lib/libiotkit-comm/iotkit-comm_mdns.c)
SET(MQTTPUBSUB_CLIENT_SUBSCRIBE_FAIL test_mqttpubsub_client_subscribe_fail.c ../../lib/plugins/libmqttpubsub/mqtt-client.c ../../lib/cJSON/cJSON.c ../../lib/libiotkit-comm/iotkit-comm.c ../../lib/libiotkit-comm/iotkit-comm_mdns.c)
SET(MQTTPUBSUB_CLIENT_SUBSCRIBE_SUCCESS test_mqttpubsub_client_subscribe_success.c ../../lib/plugins/libmqttpubsub/mqtt-client.c ../../lib/cJSON/cJSON.c ../../lib/libiotkit-comm/iotkit-comm.c ../../lib/libiotkit-comm/iotkit-comm_mdns.c)
SET(MQTTPUBSUB_CLIENT_UNSUBSCRIBE_SUCCESS test_mqttpubsub_client_unsubscribe_success.c ../../lib/plugins/libmqttpubsub/mqtt-client.c ../../lib/cJSON/cJSON.c ../../lib/libiotkit-comm/iotkit-comm.c ../../lib/libiotkit-comm/iotkit-comm_mdns.c)

# create the executables
ADD_EXECUTABLE(mqttpubsub_client_publish_success ${MQTTPUBSUB_CLIENT_PUBLISH_SUCCESS})
ADD_EXECUTABLE(mqttpubsub_client_publish_fail ${MQTTPUBSUB_CLIENT_PUBLISH_FAIL})
ADD_EXECUTABLE(mqttpubsub_client_connect_success ${MQTTPUBSUB_CLIENT_CONNECT_SUCCESS})
ADD_EXECUTABLE(mqttpubsub_client_subscribe_fail ${MQTTPUBSUB_CLIENT_SUBSCRIBE_FAIL})
ADD_EXECUTABLE(mqttpubsub_client_subscribe_success ${MQTTPUBSUB_CLIENT_SUBSCRIBE_SUCCESS})
ADD_EXECUTABLE(mqttpubsub_client_unsubscribe_success ${MQTTPUBSUB_CLIENT_UNSUBSCRIBE_SUCCESS})


SET_TARGET_PROPERTIES(mqttpubsub_client_publish_success PROPERTIES LINKER_LANGUAGE C)
SET_TARGET_PROPERTIES(mqttpubsub_client_publish_fail PROPERTIES LINKER_LANGUAGE C)
SET_TARGET_PROPERTIES(mqttpubsub_client_connect_success PROPERTIES LINKER_LANGUAGE C)
SET_TARGET_PROPERTIES(mqttpubsub_client_subscribe_fail PROPERTIES LINKER_LANGUAGE C)
SET_TARGET_PROPERTIES(mqttpubsub_client_subscribe_success PROPERTIES LINKER_LANGUAGE C)
SET_TARGET_PROPERTIES(mqttpubsub_client_unsubscribe_success PROPERTIES LINKER_LANGUAGE C)


# Link the executables
TARGET_LINK_LIBRARIES(mqttpubsub_client_publish_success paho-mqtt3cs dns_sd m dl gcov)
TARGET_LINK_LIBRARIES(mqttpubsub_client_publish_fail paho-mqtt3cs dns_sd m dl gcov)
TARGET_LINK_LIBRARIES(mqttpubsub_client_connect_success paho-mqtt3cs dns_sd m dl gcov)
TARGET_LINK_LIBRARIES(mqttpubsub_client_subscribe_fail paho-mqtt3cs dns_sd m dl gcov)
TARGET_LINK_LIBRARIES(mqttpubsub_client_subscribe_success paho-mqtt3cs dns_sd m dl gcov)
TARGET_LINK_LIBRARIES(mqttpubsub_client_unsubscribe_success paho-mqtt3cs dns_sd m dl gcov)


# Add the Test
ADD_TEST(mqttpubsub_client_publish_success ${CTEST_BINARY_DIRECTORY}/mqttpubsub_client_publish_success)
ADD_TEST(mqttpubsub_client_publish_fail ${CTEST_BINARY_DIRECTORY}/mqttpubsub_client_publish_fail)
ADD_TEST(mqttpubsub_client_connect_success ${CTEST_BINARY_DIRECTORY}/mqttpubsub_client_connect_success)
ADD_TEST(mqttpubsub_client_subscribe_fail ${CTEST_BINARY_DIRECTORY}/mqttpubsub_client_subscribe_fail)
ADD_TEST(mqttpubsub_client_subscribe_success ${CTEST_BINARY_DIRECTORY}/mqttpubsub_client_subscribe_success)
ADD_TEST(mqttpubsub_client_unsubscribe_success ${CTEST_BINARY_DIRECTORY}/mqttpubsub_client_unsubscribe_success)
