# overwrite to 5.18 version and its checksum

PV = "5.18"

SRC_URI[md5sum] = "9c71404f86287cd9441e86783be3aae1"
SRC_URI[sha256sum] = "8c8bb13db83d0de6c85def2da8dfa0a758aff595405fb57a00719ed94d558340"

# to get bluetooth.conf
FILESEXTRAPATHS_prepend := "${THISDIR}/files/"

# few overwrite for 5.18 version
SRC_URI = "\
    ${KERNELORG_MIRROR}/linux/bluetooth/bluez-${PV}.tar.xz \
    file://bluetooth.conf \
"

PACKAGECONFIG[alsa] = ""

EXTRA_OECONF = "\
  --enable-sixaxis \
  --enable-tools \
  --disable-cups \
  --enable-test \
  --enable-datafiles \
  ${@base_contains('DISTRO_FEATURES', 'systemd', '--with-systemdsystemunitdir=${systemd_unitdir}/system/', '--disable-systemd', d)} \
  --enable-library \
"

do_install_append() {
	install -d ${D}${sysconfdir}/bluetooth/
	if [ -f ${S}/profiles/audio/audio.conf ]; then
	    install -m 0644 ${S}/profiles/audio/audio.conf ${D}/${sysconfdir}/bluetooth/
	fi
	if [ -f ${S}/profiles/proximity/proximity.conf ]; then
	    install -m 0644 ${S}/profiles/proximity/proximity.conf ${D}/${sysconfdir}/bluetooth/
	fi
	if [ -f ${S}/profiles/network/network.conf ]; then
	    install -m 0644 ${S}/profiles/network/network.conf ${D}/${sysconfdir}/bluetooth/
	fi
	if [ -f ${S}/profiles/input/input.conf ]; then
	    install -m 0644 ${S}/profiles/input/input.conf ${D}/${sysconfdir}/bluetooth/
	fi
	if [ -f ${S}/src/main.conf ]; then
	    install -m 0644 ${S}/src/main.conf ${D}/${sysconfdir}/bluetooth/
	fi
	# at_console doesn't really work with the current state of OE, so punch some more holes so people can actually use BT
	install -m 0644 ${WORKDIR}/bluetooth.conf ${D}/${sysconfdir}/dbus-1/system.d/
}
