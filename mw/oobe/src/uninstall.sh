#!/bin/sh

rm -rf /usr/lib/edison_config_tools

systemctl stop edison_config
systemctl disable edison_config
rm /lib/systemd/system/edison_config.service

systemctl daemon-reload

rm /usr/bin/configure_edison

echo ""
echo ""
echo "UNINSTALL COMPLETE"
echo ""
echo "Feel free to delete this folder or keep it here."
