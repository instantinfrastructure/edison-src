
INCLUDE_DIRECTORIES(
  ../../lib/cJSON
  ../../lib/libiotkit-comm
)

# set the executable names
SET(DASHBOARD dashboard)
SET(SENSOR sensor)
SET(THERMOSTAT thermostat)

# set the source files list
SET(DASHBOARD_LIST dashboard.c ../../lib/cJSON/cJSON.c ../../lib/libiotkit-comm/iotkit-comm.c ../../lib/libiotkit-comm/iotkit-comm_mdns.c)
SET(SENSOR_LIST temperatureSensor.c ../../lib/cJSON/cJSON.c ../../lib/libiotkit-comm/iotkit-comm.c ../../lib/libiotkit-comm/iotkit-comm_mdns.c)
SET(THERMOSTAT_LIST thermostat.c ../../lib/cJSON/cJSON.c ../../lib/libiotkit-comm/iotkit-comm.c ../../lib/libiotkit-comm/iotkit-comm_mdns.c)

# create the executables
ADD_EXECUTABLE(${DASHBOARD} ${DASHBOARD_LIST})
ADD_EXECUTABLE(${SENSOR} ${SENSOR_LIST})
ADD_EXECUTABLE(${THERMOSTAT} ${THERMOSTAT_LIST})

SET_TARGET_PROPERTIES(${DASHBOARD} PROPERTIES LINKER_LANGUAGE C)
SET_TARGET_PROPERTIES(${SENSOR} PROPERTIES LINKER_LANGUAGE C)
SET_TARGET_PROPERTIES(${THERMOSTAT} PROPERTIES LINKER_LANGUAGE C)

# Link the executables
TARGET_LINK_LIBRARIES(${DASHBOARD} zmq dns_sd m dl)
TARGET_LINK_LIBRARIES(${SENSOR} zmq dns_sd m dl)
TARGET_LINK_LIBRARIES(${THERMOSTAT} zmq dns_sd m dl)
