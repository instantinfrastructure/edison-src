SUMMARY = "Low Level Skeleton Library for Communication on Intel platforms"
SECTION = "libs"
AUTHOR = "Brendan Le Foll, Tom Ingleby"

LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://COPYING;md5=e8db6501ed294e65418a933925d12058"

# git is required to get a good version from git describe
DEPENDS = "nodejs swig-native"

# URI should point to some external http:// server
SRC_URI = "file://mraa_0.4.4c.tar.gz"
SRC_URI[md5] = "6431e979cb2d44e55a4dc6e55539d447"
SRC_URI[sha256] = "9c2b4502eed98e83be85b950cbe033d195ddb446f51678431fe1e240a9c40a30"

inherit distutils-base pkgconfig python-dir cmake

FILES_${PN}-doc += "${datadir}/mraa/examples/"

FILES_${PN}-dbg += "${libdir}/node_modules/mraajs/.debug/ \
                    ${PYTHON_SITEPACKAGES_DIR}/.debug/"

do_compile_prepend () {
  # when yocto builds in ${D} it does not have access to ../git/.git so git
  # describe --tags fails. In order not to tag our version as dirty we use this
  # trick
  sed -i 's/-dirty//' src/version.c
}
