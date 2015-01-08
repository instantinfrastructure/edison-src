#!/bin/bash

echo "*** Start creating the dev tools ipk packages ***"

top_repo_dir=$(dirname $(dirname $(dirname $(readlink -f $0))))

cd $top_repo_dir/build

bitbake peeknpoke
bitbake powertop
bitbake bonnie++
bitbake wfa-tool

rm -rf devtools_packages
mkdir devtools_packages

cp $top_repo_dir/build/tmp/deploy/ipk/core2-32/peeknpoke_1.1-r0_core2-32.ipk devtools_packages/
cp $top_repo_dir/build/tmp/deploy/ipk/core2-32/powertop_2.5-r0_core2-32.ipk devtools_packages/
cp $top_repo_dir/build/tmp/deploy/ipk/core2-32/bonnie++_1.03c-r0_core2-32.ipk devtools_packages/
cp $top_repo_dir/build/tmp/deploy/ipk/core2-32/wfa-tool_r4.0-0_core2-32.ipk devtools_packages/
