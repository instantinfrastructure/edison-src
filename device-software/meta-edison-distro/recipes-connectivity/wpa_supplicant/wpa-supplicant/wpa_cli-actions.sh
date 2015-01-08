#!/bin/sh
#
# This script file is passed as parameter to wpa_cli, started as a daemon,
# so that the wpa_supplicant events are sent to this script
# and actions executed, like :
#    - start DHCP client when Wifi is connected.
#    - stop DHCP client when Wifi is disconnected.
#
# This script skips events if connmand (connman.service) is started
# Indeed, it is considered that the Wifi connection is managed through
# connmand and not wpa_cli
#

IFNAME=$1
CMD=$2

kill_daemon() {
    NAME=$1
    PF=$2

    if [ ! -r $PF ]; then
        return
    fi

    PID=`cat $PF`
    if [ $PID -gt 0 ]; then
        if ps | grep $NAME | grep $PID; then
            kill $PID
        fi
    fi
    if [ -r $PF ]; then
        # file can be removed by the deamon when killed
        rm $PF
    fi
}

echo "event $CMD received from wpa_supplicant"

# if connman is started, ignore the connmand.
# DHCP connection is triggerd by connman
if [ `systemctl is-active connman` == "active" ] ; then
    echo "event $CMD ignored because is started"
    exit 0
fi

if [ "$CMD" = "CONNECTED" ]; then
    kill_daemon udhcpc /var/run/udhcpc-$IFNAME.pid
    udhcpc -i $IFNAME -p /var/run/udhcpc-$IFNAME.pid -S
fi

if [ "$CMD" = "DISCONNECTED" ]; then
    kill_daemon udhcpc /var/run/udhcpc-$IFNAME.pid
    ifconfig $IFNAME 0.0.0.0
fi

