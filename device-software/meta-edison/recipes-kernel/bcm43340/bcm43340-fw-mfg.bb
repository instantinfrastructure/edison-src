DESCRIPTION = "MFG Firmware files for use with Linux kernel and FCC/EC specific tests"
SECTION = "kernel"

FILESEXTRAPATHS_prepend := "${EDISONREPO_TOP_DIR}/broadcom_cws/wlan/firmware/"

SRC_URI = "file://bcmdhd_mfg.cal_4334x_b0 \
           file://fw_bcmdhd_mfg.bin_4334x_b0"

LICENSE = "CLOSED"

PV = "6.20.190"
PR = "r2"

S = "${WORKDIR}"

inherit allarch update-alternatives

FILESDIR = "${FILE_DIRNAME}/files/"

do_install() {
        install -v -d  ${D}/etc/firmware/
        install -m 0755 bcmdhd_mfg.cal_4334x_b0 ${D}/etc/firmware/bcmdhd_mfg.cal
        install -m 0755 fw_bcmdhd_mfg.bin_4334x_b0 ${D}/etc/firmware/fw_bcmdhd_mfg.bin
}
