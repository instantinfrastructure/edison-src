#!/bin/bash

# This script will call a couple of bitbake targets to clean up the sstate
# (Yocto cache) for some projects.
# Calling this script before each build allows to work around some limitations
# of using the external_src / repo / gerrit workflow.

top_repo_dir=$(dirname $(dirname $(dirname $(readlink -f $0))))

cd $top_repo_dir/build/
bitbake -c cleansstate virtual/kernel
bitbake -c cleansstate u-boot
bitbake -c cleansstate bcm43340-mod
bitbake -c cleansstate nodejs
bitbake -c cleansstate nodejs-native
bitbake -c cleansstate mdns
bitbake -c cleansstate paho-mqtt
bitbake -c cleansstate rsmb
bitbake -c cleansstate zeromq
bitbake -c cleansstate iotkit-comm-c
bitbake -c cleansstate iotkit-comm-js
bitbake -c cleansstate iotkit-agent
bitbake -c cleansstate oobe
bitbake -c cleansstate mraa
bitbake -c cleansstate upm
