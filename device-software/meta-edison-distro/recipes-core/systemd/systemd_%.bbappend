FILESEXTRAPATHS_prepend := "${THISDIR}/files:"
SRC_URI += "file://journald.conf \
            file://timesyncd.conf \
            file://system.conf \
            file://systemd-reboot-service.patch \
            file://hsu-pm-runtime.service \
            file://spi-disable-pm-runtime.service"

do_install_append() {
    # Push the custom conf files on target
    install -m 0644 ${WORKDIR}/journald.conf ${D}${sysconfdir}/systemd
    install -m 0644 ${WORKDIR}/system.conf ${D}${sysconfdir}/systemd
    install -m 0644 ${WORKDIR}/timesyncd.conf ${D}${sysconfdir}/systemd

    # enable timesyncd service
    install -d ${D}${sysconfdir}/systemd/system/sysinit.target.wants
    ln -sf ${systemd_unitdir}/system/systemd-timesyncd.service \
            ${D}${sysconfdir}/systemd/system/sysinit.target.wants/systemd-timesyncd.service

    # enable a custom service to enable the hsu driver's runtime pm
    install -d ${D}${sysconfdir}/systemd/system/default.target.wants
    install -m 0644 ${WORKDIR}/hsu-pm-runtime.service \
            ${D}${systemd_unitdir}/system/hsu-pm-runtime.service

    # enable a custom service to disable the ssp_spi driver's runtime pm
    install -d ${D}${sysconfdir}/systemd/system/default.target.wants
    install -m 0644 ${WORKDIR}/spi-disable-pm-runtime.service \
            ${D}${systemd_unitdir}/system/spi-disable-pm-runtime.service
    ln -sf ${systemd_unitdir}/system/spi-disable-pm-runtime.service \
            ${D}${sysconfdir}/systemd/system/default.target.wants/spi-disable-pm-runtime.service

}
