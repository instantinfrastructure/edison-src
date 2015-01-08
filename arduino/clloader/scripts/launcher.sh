#!/bin/sh
EDISON_PATH="/opt/edison"
CLLOADER="$EDISON_PATH/clloader"
CLLOADER_OPTS="--escape --binary --zmodem --disable-timeouts"

mytrap()
{
  kill -KILL $clPID
  keepgoing=false
}

trap 'mytrap' USR1

keepgoing=true
while $keepgoing
do
  $CLLOADER $CLLOADER_OPTS < /dev/ttyGS0 > /dev/ttyGS0 & clPID=$!
  wait $clPID
  usleep 200000
done

