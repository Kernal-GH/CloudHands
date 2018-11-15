#!/bin/bash
#
#      Filename: make_dpdk_package.sh
#
#        Author: csp001314@163.com
#   Description: ---
#        Create: 2017-01-18 15:01:53
# Last Modified: 2018-06-19 18:25:25
#

if [ $# -lt 1 ]; then

	echo "usage:<dpdk version>"
	exit
fi

DPDK_VER=$1
DPDK_LOCATION=http://fast.dpdk.org/rel/dpdk-${DPDK_VER}.tar.xz
DPDK=dpdk-${DPDK_VER}.tar.xz
DPDK_NAME=dpdk-${DPDK_VER}
DPDK_CACHE_DIR=/tmp/.dpdk/

rm -rf build
mkdir build

if [ ! -d $DPDK_CACHE_DIR ]; then
	mkdir -p $DPDK_CACHE_DIR
fi

if [ ! -f $DPDK_CACHE_DIR/$DPDK ]; then
	wget $DPDK_LOCATION -P $DPDK_CACHE_DIR
fi

tar -xvf  $DPDK_CACHE_DIR/$DPDK -C build

if [ -d build/dpdk-stable-${DPDK_VER} ]; then

   mv build/dpdk-stable-${DPDK_VER} build/${DPDK_NAME}
fi

cp -r $DPDK_NAME/* build/$DPDK_NAME/

cd build

tar -czvf dpdk-${DPDK_VER}.tar.gz $DPDK_NAME

rm -rf $DPDK_NAME
cd ../



