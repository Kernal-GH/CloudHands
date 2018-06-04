#!/bin/sh
#
#      Filename: install.sh
#
#        Author: csp001314@163.com
#   Description: ---
#        Create: 2016-11-03 20:09:47
# Last Modified: 2017-06-01 17:12:02
#

[ `id -u` -ne 0 ] && {
	echo "You must run this by root" >&2
	exit 1
}

dpdk_ver=17.02
msgpack_name=msgpack-c-master
apr_name=apr-1.5.2
apr_util_name=apr-util-1.5.4

apr_install_prefix=/usr/local/apr
dpdk_install_prefix=/usr/local/dpdk

install_apr(){

   echo "install apr--------------------------------"

   if [ ! -f /usr/local/apr/bin/apr-1-config ]; then
	   cd tar
	   tar -zxvf ${apr_name}.tar.gz
	   
	   cd $apr_name
	   ./configure --with-prefix=$apr_install_prefix
	   make
	   make install
	   cd ../../
	fi

	if [ ! -f /usr/local/apr/bin/apu-1-config ]; then
		cd tar 
		tar -zxvf ${apr_util_name}.tar.gz
		cd $apr_util_name
		./configure --with-apr=$apr_install_prefix
		make
		make install
		cd ../../
	fi

	if [ ! -f /etc/ld.so.conf.d/apr.conf ]; then
		echo "/usr/local/apr/lib" >/etc/ld.so.conf.d/apr.conf
	fi
    
	ldconfig

}

install_msgpack(){

   echo "install msgpack--------------------------------"

   if [ ! -f /usr/local/lib/libmsgpack.a ]; then
	   cd tar
	   unzip ${msgpack_name}.zip
	   
	   cd $msgpack_name
	   ./bootstrap
	   ./configure
	   make
	   make install
	   cd ../../
	fi
	ldconfig
}

install_dpdk(){

	echo "install dpdk----------------------"

    cd tar
    tar -xvf dpdk-${dpdk_ver}.tar.gz

    cd dpdk-${dpdk_ver}

    make install T=x86_64-native-linuxapp-gcc
    cp -rf x86_64-native-linuxapp-gcc/* $dpdk_install_prefix
    cp -rf tools $dpdk_install_prefix
    cd ../../
}

install_proto(){

	make -C  CloudHands/protocols/tproto
	make -C  CloudHands/protocols/tproto/http
	make -C  CloudHands/protocols/tproto/mail
	make -C  CloudHands/protocols/tproto/debug
	make -C  CloudHands/protocols/tproto/ftp
	make -C  CloudHands/protocols/tproto/telnet

	make -C  CloudHands/protocols/uproto
	make -C  CloudHands/protocols/uproto/dns
	
	make -C  CloudHands/protocols/

}

install_tool(){

	make -C CloudHands/tool
}

install_cloudhands(){
    
     export RTE_SDK=`pwd`/tar/dpdk-${dpdk_ver}

     make -C CloudHands
     cp -f CloudHands/core/MakefileLib CloudHands/core/Makefile
     make -C CloudHands/core

     make -C CloudHands/test/

	 install_proto
	 install_tool

     if [ ! -d $dpdk_install_prefix/CloudHands/protocols ];then
         mkdir -p $dpdk_install_prefix/CloudHands/protocols
     fi

     cp -rf CloudHands/core/build/app/* $dpdk_install_prefix/CloudHands
     cp -rf CloudHands/test/build/app/* $dpdk_install_prefix/CloudHands
     cp -rf CloudHands/protocols/proto_main $dpdk_install_prefix/CloudHands/protocols
	 cp -rf CloudHands/conf $dpdk_install_prefix/CloudHands
	 cp -rf CloudHands/protocols/conf $dpdk_install_prefix/CloudHands/protocols
	 cp -rf CloudHands/bin/* $dpdk_install_prefix/tools
	 cp -rf CloudHands/tool/preader $dpdk_install_prefix/tools
}

install_all(){
	install_msgpack
	install_apr
	install_dpdk

	install_cloudhands
}

install_all

