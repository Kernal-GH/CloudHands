#!/bin/sh
#
#      Filename: install.sh
#
#        Author: csp001314@163.com
#   Description: ---
#        Create: 2016-11-03 20:09:47
# Last Modified: 2018-10-29 18:56:07
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

	export RTE_TARGET=/usr/local/dpdk/dpdk_src

    cd ../../
}

comp_lib(){

	make -C CloudHands/lib/common
	make -C CloudHands/lib/ptables
	make -C CloudHands/lib/shm
	make -C CloudHands/lib/thread
	make -C CloudHands/lib/fstore
	make -C CloudHands/lib/redis
	make -C CloudHands/lib/lua
	make -C CloudHands/packet
	make -C CloudHands/MProcess
	make -C CloudHands/tcp/app
	make -C CloudHands/tcp/app/http
	make -C CloudHands/tcp/app/mail
	make -C CloudHands/tcp/app/ftp
	make -C CloudHands/tcp/app/telnet
	make -C CloudHands/tcp/app/smon
	make -C CloudHands/tcp/app/ssh

	cp -rf  CloudHands/tcp/app/http/*.o CloudHands/tcp/app/
	cp -rf  CloudHands/tcp/app/mail/*.o CloudHands/tcp/app/
	cp -rf  CloudHands/tcp/app/mail/smtp/*.o CloudHands/tcp/app/
	cp -rf  CloudHands/tcp/app/mail/imap/*.o CloudHands/tcp/app/
	cp -rf  CloudHands/tcp/app/mail/pop3/*.o CloudHands/tcp/app/
	cp -rf  CloudHands/tcp/app/ftp/*.o CloudHands/tcp/app/
	cp -rf  CloudHands/tcp/app/telnet/*.o CloudHands/tcp/app/
	cp -rf  CloudHands/tcp/app/smon/*.o CloudHands/tcp/app/
	cp -rf  CloudHands/tcp/app/ssh/*.o CloudHands/tcp/app/


	make -C CloudHands/udp/app
	make -C CloudHands/udp/app/dns
	make -C CloudHands/udp/app/tftp
	make -C CloudHands/udp/app/smon
	cp -rc  CloudHands/udp/app/dns/*.o CloudHands/udp/app/
	cp -rc  CloudHands/udp/app/tftp/*.o CloudHands/udp/app/
	cp -rc  CloudHands/udp/app/smon/*.o CloudHands/udp/app/
	make -C CloudHands/statistic
	make -C CloudHands/SessionMonitor
	make -C CloudHands/WBList

}

prepare_dir(){

	if [ ! -d $dpdk_install_prefix/CloudHands/bin ];then
         mkdir -p $dpdk_install_prefix/CloudHands/bin
    fi

	if [ ! -d $dpdk_install_prefix/CloudHands/sample ];then
         mkdir -p $dpdk_install_prefix/CloudHands/sample
    fi

	if [ ! -d $dpdk_install_prefix/CloudHands/KafkaSink ];then
         mkdir -p $dpdk_install_prefix/CloudHands/KafkaSink
    fi

    if [ ! -d $dpdk_install_prefix/CloudHands/lib ];then
         mkdir -p $dpdk_install_prefix/CloudHands/lib
    fi

    if [ ! -d $dpdk_install_prefix/CloudHands/lua ];then
         mkdir -p $dpdk_install_prefix/CloudHands/lua
    fi
}


install_cloudhands(){

	echo "install cloudhands-----------------------------"

	prepare_dir

	make -C CloudHands/PDispatcher
	make -C CloudHands/tcp
	make -C CloudHands/udp
	make -C CloudHands/StreamAnalyze
	make -C CloudHands/statistic -f MakefileDump
	make -C CloudHands/SessionMonitor -f MakefileCmd
	make -C CloudHands/WBList -f MakefileIPCmd
	make -C CloudHands/WBList -f MakefileStrCmd
	make -C CloudHands/lib/redis/ -f MakefileIPBitmapCmd
	make -C CloudHands/lib/shm/ -f MakefileSHMInfoDump

	cp -rf CloudHands/conf $dpdk_install_prefix/CloudHands
	cp -rf CloudHands/PDispatcher/PDispatcher $dpdk_install_prefix/CloudHands/bin
	cp -rf CloudHands/tcp/TCPMain $dpdk_install_prefix/CloudHands/bin
	cp -rf CloudHands/udp/UDPMain $dpdk_install_prefix/CloudHands/bin
	cp -rf CloudHands/StreamAnalyze/SAMain $dpdk_install_prefix/CloudHands/bin
	cp -rf CloudHands/statistic/StatDump $dpdk_install_prefix/CloudHands/bin
	cp -rf CloudHands/bin/* $dpdk_install_prefix/CloudHands/bin
	cp -rf CloudHands/SessionMonitor/SMonCmd $dpdk_install_prefix/CloudHands/bin
	cp -rf CloudHands/WBList/WBListIPCmd $dpdk_install_prefix/CloudHands/bin
	cp -rf CloudHands/WBList/WBListStrCmd $dpdk_install_prefix/CloudHands/bin
	cp -rf CloudHands/lib/redis/IPBitmapCmd $dpdk_install_prefix/CloudHands/bin
	cp -rf CloudHands/lib/shm/SHMInfoDump $dpdk_install_prefix/CloudHands/bin

    echo "install lua script"
    cp -rf CloudHands/lib/lua/base/* $dpdk_install_prefix/CloudHands/lua
    cp -rf CloudHands/lib/lua/statistic/* $dpdk_install_prefix/CloudHands/lua
    cp -rf CloudHands/StreamAnalyze/lua/* $dpdk_install_prefix/CloudHands/lua

}

install_java_api(){

    make -C CloudHands/java/jni
    cp -rf  CloudHands/java/jni/libCloudHandsJNI.so $dpdk_install_prefix/CloudHands/lib
    cp -rf  CloudHands/java/jni/libCloudHandsJNI.so /usr/lib/
    mvn install -f CloudHands/java/CloudHandsJAPI/pom.xml
}

install_sample(){

	mvn package -f CloudHands/sample/java/CloudHandsJAPISample/pom.xml
	cp -rf CloudHands/sample/java/CloudHandsJAPISample/target/CloudHandsJAPISample-1.0.0.jar $dpdk_install_prefix/CloudHands/sample
	cp -rf CloudHands/sample/java/CloudHandsJAPISample/bin/*  $dpdk_install_prefix/CloudHands/sample
	cp -rf CloudHands/sample/java/CloudHandsJAPISample/conf/* $dpdk_install_prefix/CloudHands/sample
}

install_kafkaSink(){

	mvn package -f CloudHands/java/CloudHandsJAPIKafka/pom.xml
	cp -rf CloudHands/java/CloudHandsJAPIKafka/target/CloudHandsJAPIKafka-1.0.jar $dpdk_install_prefix/CloudHands/KafkaSink
	cp -rf CloudHands/java/CloudHandsJAPIKafka/bin/*  $dpdk_install_prefix/CloudHands/KafkaSink
	cp -rf CloudHands/java/CloudHandsJAPIKafka/conf/* $dpdk_install_prefix/CloudHands/KafkaSink
}

install_all(){

	source /etc/profile
	install_dpdk
	comp_lib
	install_cloudhands
    install_java_api
	install_kafkaSink
	install_sample
}

install_all

