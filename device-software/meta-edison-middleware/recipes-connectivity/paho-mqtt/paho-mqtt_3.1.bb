DESCRIPTION = "Paho MQTT - user libraries for the MQTT and MQTT-SN protocols"
LICENSE = "EPL-1.0 | EDL-1.0"

S = "${EDISONREPO_TOP_DIR}/mw/mqtt"

LIC_FILES_CHKSUM = " \
		 file://edl-v10;md5=3adfcc70f5aeb7a44f3f9b495aa1fbf3 \
		 file://epl-v10;md5=659c8e92a40b6df1d9e3dccf5ae45a08 \
"

do_compile() {
    oe_runmake
}

do_install() {
    install -d ${D}${libdir}
    oe_libinstall -C build/output -so libpaho-mqtt3a ${D}${libdir}
    oe_libinstall -C build/output -so libpaho-mqtt3as ${D}${libdir}
    oe_libinstall -C build/output -so libpaho-mqtt3c  ${D}${libdir}
    oe_libinstall -C build/output -so libpaho-mqtt3cs ${D}${libdir}
    install -d ${D}${includedir}
    install -m 644 src/MQTTAsync.h ${D}${includedir}
    install -m 644 src/MQTTClient.h ${D}${includedir}
    install -m 644 src/MQTTClientPersistence.h ${D}${includedir}
}

DEPENDS = "openssl"

