DESCRIPTION = "Mosquitto - lightweight server implementation of the MQTT and MQTT-SN protocols"
LICENSE = "EPL-1.0 | EDL-1.0"

S = "${EDISONREPO_TOP_DIR}/mw/rsmb"

SRC_URI = "file://rsmb.service"

LIC_FILES_CHKSUM = " \
        file://edl-v10;md5=3adfcc70f5aeb7a44f3f9b495aa1fbf3 \
        file://epl-v10;md5=659c8e92a40b6df1d9e3dccf5ae45a08 \
"

do_compile() {
    cd rsmb/src
    oe_runmake broker_mqtts32
}

do_install() {
    cd rsmb/src
    install -d ${D}${bindir}
    install -m 0755 broker_mqtts32 ${D}${bindir}
    install -d ${D}${datadir}/${BPN}
    install -m 0444 Messages.1.3.0.2 ${D}${datadir}/${BPN}

    install -d ${D}${systemd_unitdir}/system/
    install -m 0644 ${WORKDIR}/rsmb.service ${D}${systemd_unitdir}/system/
}

inherit systemd

SYSTEMD_SERVICE_${PN} = "rsmb.service"

FILES_${PN} += "${systemd_unitdir}/system/rsmb.service ${datadir}/${BPN}/*"

