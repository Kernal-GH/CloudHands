#!/bin/sh
#
#      Filename: install.sh
#
#        Author: csp001314@163.com
#   Description: ---
#        Create: 2016-11-03 20:09:47
# Last Modified: 2018-06-19 18:25:01
#

[ `id -u` -ne 0 ] && {
	echo "You must run this by root" >&2
	exit 1
}


self_path=$(cd $(dirname "$0"); pwd)

. $self_path/dpdk.sh

dpdk_install_prefix=/usr/local/dpdk/

if [ ! -d ${dpdk_install_prefix} ]; then

   mkdir -p ${dpdk_install_prefix}
fi

install_dpdk(){

	echo "install dpdk----------------------"

    cd tar
    tar -xvf dpdk-${dpdk_ver}.tar.gz

    cd dpdk-${dpdk_ver}

    rm -rf /usr/src/dpdk
    mkdir -p /usr/src/dpdk
    cp -rf * /usr/src/dpdk

    make -C /usr/src/dpdk install T=x86_64-native-linuxapp-gcc
    
    cp -rf /usr/src/dpdk/x86_64-native-linuxapp-gcc/* $dpdk_install_prefix
    cp -rf /usr/src/dpdk/usertools $dpdk_install_prefix
    ln -s /usr/src/dpdk/x86_64-native-linuxapp-gcc /usr/local/dpdk/dpdk_src

    cd ../../
}

comp_lib(){

	make -C CloudHands/lib/common
	make -C CloudHands/lib/ptables
	make -C CloudHands/lib/shm
	make -C CloudHands/lib/thread
	make -C CloudHands/lib/fstore
	make -C CloudHands/packet
	make -C CloudHands/MProcess
	make -C CloudHands/app
	cp -rf  CloudHands/app/tcp_app/*.o CloudHands/app/
	cp -rf  CloudHands/app/tcp_app/debug/*.o CloudHands/app/
	cp -rf  CloudHands/app/tcp_app/http/*.o CloudHands/app/
	cp -rf  CloudHands/app/tcp_app/ftp/*.o CloudHands/app/
	cp -rf  CloudHands/app/tcp_app/imap/*.o CloudHands/app/
	cp -rf  CloudHands/app/tcp_app/pop3/*.o CloudHands/app/
	cp -rf  CloudHands/app/tcp_app/smtp/*.o CloudHands/app/
	cp -rf  CloudHands/app/tcp_app/telnet/*.o CloudHands/app/

	make -C CloudHands/tcp/PParse/http
	make -C CloudHands/tcp/PParse/mail
	make -C CloudHands/udp/dns
	make -C CloudHands/StreamAnalyze/statistic

}

prepare_dir(){

	if [ ! -d $dpdk_install_prefix/CloudHands/bin ];then
         mkdir -p $dpdk_install_prefix/CloudHands/bin
    fi

	if [ ! -d $dpdk_install_prefix/CloudHands/sample ];then
         mkdir -p $dpdk_install_prefix/CloudHands/sample
    fi
	
    if [ ! -d $dpdk_install_prefix/CloudHands/lib ];then
         mkdir -p $dpdk_install_prefix/CloudHands/lib
    fi
}

install_cloudhands(){

	echo "install cloudhands-----------------------------"
     
	prepare_dir

	make -C CloudHands/PDispatcher
	make -C CloudHands/tcp
	make -C CloudHands/tcp/PParse
	make -C CloudHands/udp
	make -C CloudHands/StreamAnalyze
	make -C CloudHands/StreamAnalyze/statistic -f MakefileDump

	cp -rf CloudHands/conf $dpdk_install_prefix/CloudHands
	cp -rf CloudHands/PDispatcher/PDispatcher $dpdk_install_prefix/CloudHands/bin
	cp -rf CloudHands/tcp/TCPMain $dpdk_install_prefix/CloudHands/bin
	cp -rf CloudHands/tcp/PParse/PPMain $dpdk_install_prefix/CloudHands/bin
	cp -rf CloudHands/udp/UDPMain $dpdk_install_prefix/CloudHands/bin
	cp -rf CloudHands/StreamAnalyze/SAMain $dpdk_install_prefix/CloudHands/bin
	cp -rf CloudHands/StreamAnalyze/statistic/StatDump $dpdk_install_prefix/CloudHands/bin
}

install_java_api(){

    make -C CloudHands/java/jni
    cp -rf  CloudHands/java/jni/libCloudHandsJNI.so $dpdk_install_prefix/CloudHands/lib
    mvn install -f CloudHands/java/CloudHandsJAPI/pom.xml
}

install_all(){
	install_dpdk
	comp_lib
	install_cloudhands
    install_java_api
}

install_all

