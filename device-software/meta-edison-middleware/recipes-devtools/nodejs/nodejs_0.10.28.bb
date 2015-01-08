DESCRIPTION = "nodeJS Evented I/O for V8 JavaScript"
HOMEPAGE = "http://nodejs.org"
# tools/wrk is under Apache-2.0
LICENSE = "MIT & BSD & Apache-2.0"
LIC_FILES_CHKSUM = "file://LICENSE;md5=4a31e6c424761191227143b86f58a1ef"

DEPENDS = "openssl"
DEPENDS_class-target = "nodejs-native"

SRC_URI = "git://github.com/joyent/node.git;protocol=https;branch=v0.10;tag=v0.10.28"

S = "${WORKDIR}/git"

# v8 errors out if you have set CCACHE
CCACHE = ""

ARCHFLAGS_arm = "${@bb.utils.contains('TUNE_FEATURES', 'callconvention-hard', '--with-arm-float-abi=hard', '--with-arm-float-abi=softfp', d)}"
ARCHFLAGS ?= ""

# Node is way too cool to use proper autotools, so we install two wrappers to forcefully inject proper arch cflags to workaround gypi
do_configure () {
    export LD="${CXX}"

    ./configure --prefix=${prefix} --without-snapshot ${ARCHFLAGS}
}

do_compile () {
    export LD="${CXX}"
    make BUILDTYPE=Release
}

do_install () {
    oe_runmake install DESTDIR=${D}
}

do_install_append_class-target () {
    # install node-gyp node hedaers in /usr/include/node-gyp/
    cd ${D}/${libdir}/node_modules/npm/node_modules/node-gyp/
    export HOME=${D}/usr/include/node-gyp

    sed -i 's/\.node-gyp//' lib/node-gyp.js

    # configure http proxy if neccessary
    if [ -n "${http_proxy}" ]; then
        ${STAGING_BINDIR_NATIVE}/node bin/node-gyp.js --verbose --proxy=${http_proxy} install
    elif [ -n "${HTTP_PROXY}" ]; then
        ${STAGING_BINDIR_NATIVE}/node bin/node-gyp.js --verbose --proxy=${HTTP_PROXY} install
    else
        ${STAGING_BINDIR_NATIVE}/node bin/node-gyp.js --verbose install
    fi
}

pkg_postinst_${PN} () {
    sed -e '/^PATH=/aNODE_PATH=\/usr\/lib\/node_modules\/' \
        -e 's/\(^export\)\(.*\)/\1 NODE_PATH\2/' \
        -i $D/etc/profile
}

pkg_prerm_${PN} () {
    sed -e '/^NODE_PATH.*$/d' \
        -e 's/\(^export\)\(.*\)\(\<NODE_PATH\>\s\)\(.*\)/\1\2\4/' \
        -i $D/etc/profile
}

RDEPENDS_${PN} = "curl python-shell python-datetime python-subprocess python-crypt python-textutils \
                python-netclient python-ctypes python-misc python-compiler python-multiprocessing"

RDEPENDS_${PN}_class-native = ""

FILES_${PN} += "${libdir}/node/wafadmin ${libdir}/node_modules ${libdir}/dtrace"
BBCLASSEXTEND = "native"
