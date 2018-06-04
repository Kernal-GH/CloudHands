#!/bin/bash
#
#      Filename: install.sh
#
#        Author: gongyanlei@antell.com.cn
#   Description: ---
#        Create: 2017-09-08 17:22:23
# Last Modified: 2017-09-27 18:04:52
#

dpdk_ver=17.02
dpdk_install_prefix=/opt/antell/dpdk

install_dpdk(){

	echo "install dpdk----------------------"

    cd tar
	echo "Extract dpdk package"
    tar -zxf dpdk-${dpdk_ver}.tar.gz

    cd dpdk-${dpdk_ver}

    make install T=x86_64-native-linuxapp-gcc

    \cp -rf x86_64-native-linuxapp-gcc/* $dpdk_install_prefix
	\cp -fr usertools $dpdk_install_prefix

    cd ../../
}

install_tool(){

	make -C tool
}

install_cloudhands(){
    
     export RTE_SDK=`pwd`/tar/dpdk-${dpdk_ver}

     make
	 cd core 
	 make -f MakefileLib
	 cd ..

	 install_tool

     \cp -rf core/build/app/* $dpdk_install_prefix/CloudHands
	 \cp -rf conf $dpdk_install_prefix/CloudHands
	 \cp -rf bin/* $dpdk_install_prefix/tools
	 \cp -rf tool/preader $dpdk_install_prefix/tools
	 chmod 777 -R $dpdk_install_prefix
}

install_all(){
	mkdir -p $dpdk_install_prefix/CloudHands
	mkdir -p $dpdk_install_prefix/tools

	install_dpdk

	install_cloudhands

	if [ ! -d /opt/data/assembleFile/ ]; then
		mkdir -p /opt/data/assembleFile/
	fi
}

install_all

