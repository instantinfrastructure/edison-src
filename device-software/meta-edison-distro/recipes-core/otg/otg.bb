DESCRIPTION = "Systemd usb0 config"
LICENSE = "GPLv2+"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

SRC_URI = "file://network-gadget-init.service"

SYSTEMD_SERVICE_${PN} = "network-gadget-init.service"

RDEPENDS_${PN} = "systemd"

do_install() {
	install -d ${D}${systemd_unitdir}/system
	install -d ${D}${sysconfdir}/systemd/system/basic.target.wants
	install -c -m 0644 ${WORKDIR}/network-gadget-init.service ${D}${systemd_unitdir}/system
	sed -i -e 's,@BASE_BINDIR@,${base_bindir},g' \
		-e 's,@SBINDIR@,${sbindir},g' \
		-e 's,@BINDIR@,${bindir},g' \
		${D}${systemd_unitdir}/system/*.service

	# enable the service
	ln -sf ${systemd_unitdir}/system/network-gadget-init.service \
		${D}${sysconfdir}/systemd/system/basic.target.wants/network-gadget-init.service
}

FILES_${PN} = "${base_libdir}/systemd/system/*.service \
				${sysconfdir}"

# As this package is tied to systemd, only build it when we're also building systemd.
python () {
    if not oe.utils.contains ('DISTRO_FEATURES', 'systemd', True, False, d):
        raise bb.parse.SkipPackage("'systemd' not in DISTRO_FEATURES")
}
