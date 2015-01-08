#!/bin/bash

# On ubuntu you need to install libqt4-core:i386 and libqt4-gui:i386 to run this script

top_repo_dir=$(dirname $(dirname $(dirname $(dirname $(readlink -f $0)))))

# Cleanup previous builds
rm -rf $top_repo_dir/build/toFlash/*
mkdir -p $top_repo_dir/build/toFlash

# Copy boot partition (contains kernel and ramdisk)
cp $top_repo_dir/build/tmp/deploy/images/edison/edison-image-edison.hddimg $top_repo_dir/build/toFlash/

# Copy u-boot
cp $top_repo_dir/build/tmp/deploy/images/edison/u-boot-edison.img $top_repo_dir/build/toFlash/
cp $top_repo_dir/build/tmp/deploy/images/edison/u-boot-edison.bin $top_repo_dir/build/toFlash/

# Copy u-boot environments files binary
cp -R $top_repo_dir/build/tmp/deploy/images/edison/u-boot-envs $top_repo_dir/build/toFlash

# Copy IFWI
cp $top_repo_dir/device-software/utils/flash/ifwi/edison/*.bin $top_repo_dir/build/toFlash/

# build Ifwi file for using in DFU mode
# Remove FUP footer (144 bytes) as it's not needed when we directly write to boot partitions
for ifwi in $top_repo_dir/build/toFlash/*ifwi*.bin ;
do
    dfu_ifwi_name="`basename $ifwi .bin`-dfu.bin"
    dd if=$ifwi of=$top_repo_dir/build/toFlash/$dfu_ifwi_name bs=4194304 count=1
done

# Copy rootfs
cp $top_repo_dir/build/tmp/deploy/images/edison/edison-image-edison.ext4 $top_repo_dir/build/toFlash/

# Copy symbols files
mkdir -p $top_repo_dir/build/symbols
cp $top_repo_dir/build/tmp/deploy/images/edison/vmlinux $top_repo_dir/build/symbols/
cp $top_repo_dir/build/tmp/deploy/images/edison/u-boot-edison.bin $top_repo_dir/build/symbols/


# copy phoneflashtool xml file
cp $top_repo_dir/device-software/utils/flash/pft-config-edison.xml $top_repo_dir/build/toFlash/

# Copy flashing script
cp $top_repo_dir/device-software/utils/flash/flashall.sh $top_repo_dir/build/toFlash/
cp $top_repo_dir/device-software/utils/flash/flashall.bat $top_repo_dir/build/toFlash/
cp $top_repo_dir/device-software/utils/flash/filter-dfu-out.js $top_repo_dir/build/toFlash/

# copy OTA update
cp $top_repo_dir/device-software/utils/flash/ota_update.cmd $top_repo_dir/build/toFlash/

# Preprocess OTA script
# Compute sha1sum of each file under build/toFlash and build an array containing
# @@sha1_filename:SHA1VALUE
pth_out=$top_repo_dir/build/toFlash/
tab_size=$(for fil in $(find $pth_out -maxdepth 1 -type f -printf "%f\n") ; do sha1_hex=$(sha1sum "$pth_out$fil" | cut -c -40); echo "@@sha1_$fil:$sha1_hex" ; done ;)
# iterate the array and do tag -> value substitution in ota_update.cmd
for elem in $tab_size ; do IFS=':' read -a fld_elem <<< "$elem"; sed -i "s/${fld_elem[0]}/${fld_elem[1]}/g" $top_repo_dir/build/toFlash/ota_update.cmd; done;

# Look for mkimage tool path
uboot_default_path=$top_repo_dir/u-boot/tools/
uboot_ext_src_path=$top_repo_dir/build/tmp/work/edison-poky-linux/u-boot
if [ -d $uboot_default_path ]; then
    mkimage_tool_path=$(find $uboot_default_path -name mkimage)
else
    mkimage_tool_path=$(find $uboot_ext_src_path -name mkimage)
fi

# Convert OTA script to u-boot script
$mkimage_tool_path -a 0x10000 -T script -C none -n 'Edison Updater script' -d $top_repo_dir/build/toFlash/ota_update.cmd $top_repo_dir/build/toFlash/ota_update.scr

# Supress Preprocessed OTA script
rm $top_repo_dir/build/toFlash/ota_update.cmd

# Generates a formatted list of all packages included in the image
awk '{print $1 " " $3}' $top_repo_dir/build/tmp/deploy/images/edison/edison-image-edison.manifest > $top_repo_dir/build/toFlash/package-list.txt

echo "**** Done ***"
echo "Files ready to flash in $top_repo_dir/build/toFlash/"
echo "Run the flashall script there to start flashing."
echo "*************"
