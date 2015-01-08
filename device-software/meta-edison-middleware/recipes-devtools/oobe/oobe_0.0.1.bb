DESCRIPTION="The out-of-box configuration service"
LICENSE = "LGPLv2.1"

S = "${EDISONREPO_TOP_DIR}/mw/oobe"

LIC_FILES_CHKSUM = " \
        file://LICENSE;md5=1a6d268fd218675ffea8be556788b780 \
"

DEPENDS = "nodejs-native"

do_compile() {
}

do_install() {
   install -d ${D}${libdir}/edison_config_tools
   install -d ${D}/var/lib/edison_config_tools
   cp -r ${S}/src/public ${D}${libdir}/edison_config_tools
   install -m 0644 ${S}/src/server.js ${D}${libdir}/edison_config_tools/edison-config-server.js
   install -d ${D}${systemd_unitdir}/system/
   install -m 0644 ${S}/src/edison_config.service ${D}${systemd_unitdir}/system/
   install -d ${D}${bindir}
   install -m 0755 ${S}/src/configure_edison ${D}${bindir}
}

inherit systemd

SYSTEMD_AUTO_ENABLE = "enable"
SYSTEMD_SERVICE_${PN} = "edison_config.service"

FILES_${PN} = "${libdir}/edison_config_tools \
               ${systemd_unitdir}/system \
               /var/lib/edison_config_tools \
               ${bindir}/"

PACKAGES = "${PN}"

