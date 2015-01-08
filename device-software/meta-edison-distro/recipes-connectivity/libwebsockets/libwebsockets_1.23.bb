SUMMARY = "C library for lightweight websocket clients and servers"
DESCRIPTION = "Libwebsockets is a lightweight pure C library built \
               to use minimal CPU and memory resources, and provide \
               fast throughput in both directions."
HOMEPAGE = "http://libwebsockets.org"

LICENSE = "LGPL-2.1"
LIC_FILES_CHKSUM = "file://${S}/LICENSE;md5=041a1dec49ec8a22e7f101350fd19550"

SRC_URI = "http://git.libwebsockets.org/cgi-bin/cgit/${PN}/snapshot/${PN}-${PV}-chrome32-firefox24.tar.gz"
SRC_URI[md5sum] = "b39c559c62192128bf4c3d8eedd992a8"
SRC_URI[sha256sum] = "8d94a75ed1b69571d251e79512a424de51a411bafd57a66ccbe690b560a41359"

S = "${WORKDIR}/${PN}-${PV}-chrome32-firefox24"

DEPENDS += "openssl"

inherit cmake

do_configure() {
  # This line allows to tell openssl where its confog files are located
  export OPENSSL_CONF=${TMPDIR}/sysroots/x86_64-linux/usr/lib/ssl/openssl.cnf
  cmake_do_configure
}

PACKAGES += "${PN}-tests"

FILES_${PN}-tests += "${bindir}/libwebsockets-test* \
                      ${datadir}/libwebsockets-test-server/* \
                     "
