DESCRIPTION = "Inter of Things communication library for device-to-device and device-to-cloud messaging"
LICENSE = "LGPLv2.1"

S = "${EDISONREPO_TOP_DIR}/mw/iecf-js"

LIC_FILES_CHKSUM = " \
        file://LICENSE;md5=1a6d268fd218675ffea8be556788b780 \
"

DEPENDS = "nodejs-native zeromq mdns paho-mqtt"

do_compile () {
    # changing the home directory to the working directory, the .npmrc will be created in this directory
    export HOME=${WORKDIR}

    # does not build dev packages
    npm config set dev false

    # access npm registry using http
    npm set strict-ssl false
    npm config set registry http://registry.npmjs.org/

    # configure http proxy if neccessary
    if [ -n "${http_proxy}" ]; then
        npm config set proxy ${http_proxy}
    fi
    if [ -n "${HTTP_PROXY}" ]; then
        npm config set proxy ${HTTP_PROXY}
    fi

    # configure cache to be in working directory
    npm set cache ${WORKDIR}/npm_cache

    # clear local cache prior to each compile
    npm cache clear

    # compile and install  node modules in source directory
    npm --arch=${TARGET_ARCH} --production --verbose install
}

do_install () {
    install -d ${D}${libdir}/node_modules/iotkit-comm/
    cp -r ${S}/node_modules ${D}${libdir}/node_modules/iotkit-comm/
    install -m 644 ${S}/package.json ${D}${libdir}/node_modules/iotkit-comm/
    install -m 644 ${S}/LICENSE ${D}${libdir}/node_modules/iotkit-comm/
    install -m 644 ${S}/README.md ${D}${libdir}/node_modules/iotkit-comm/
    install -m 644 ${S}/jsdoc-conf.json ${D}${libdir}/node_modules/iotkit-comm/
    cp -r ${S}/lib ${D}${libdir}/node_modules/iotkit-comm/
    cp -r ${S}/test ${D}${libdir}/node_modules/iotkit-comm/
    cp -r ${S}/doc ${D}${libdir}/node_modules/iotkit-comm/
    install -d ${D}${datadir}/iotkit-comm/examples/node
    cp -r ${S}/example/* ${D}${datadir}/iotkit-comm/examples/node
}

INHIBIT_PACKAGE_DEBUG_SPLIT = "1"
INHIBIT_PACKAGE_STRIP = "1"

FILES_${PN} = "${libdir}/node_modules/ ${datadir}/iotkit-comm/examples"
RDEPENDS_${PN} = "nodejs zeromq mdns paho-mqtt rsmb"

PACKAGES = "${PN}"
