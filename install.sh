#!/bin/bash
#By shajf

[ `id -u` -ne 0 ] && {
	echo "You must run this by root" >&2
	exit 1
}

dpdk_install_prefix=/usr/local/dpdk
nr_pages=1024
nodes_dir=/sys/devices/system/node
huge_pages_mnt=/mnt/huge_pages
dpdk_ver=16.04

apr_location=http://mirrors.hust.edu.cn/apache/apr/apr-1.5.2.tar.gz
apr_util_location=http://mirrors.hust.edu.cn/apache/apr/apr-util-1.5.4.tar.gz
apr_name=apr-1.5.2
apr_util_name=apr-util-1.5.4

if [[ ! -d build ]];then
    mkdir build
fi

if [[ ! -d $dpdk_install_prefix ]];then
    mkdir $dpdk_install_prefix
fi

#clear build dir
rm -rf build/*

#reverse huge pages memory for dpdk
for f in `ls $nodes_dir`
do 
    [[ $f == "node"* ]] &&{
        echo $nr_pages > $nodes_dir/$f/hugepages/hugepages-2048kB/nr_hugepages
    }
done
#mount huge pages 
if [[ ! -d $huge_pages_mnt ]];then
    mkdir -p $huge_pages_mnt
fi

mount -t hugetlbfs nodev $huge_pages_mnt


#install apr&apr_util
function install_apr(){

   cur_dir=`pwd`

   if [ ! -f /tmp/${apr_name}.tar.gz ]; then
		wget -P /tmp/ $apr_location
		cd /tmp
		tar -zxvf ${apr_name}.tar.gz
		cd $apr_name
		./configure
		make
		make install
		
	fi

	if [ ! -f /tmp/${apr_util_name}.tar.gz ]; then
		wget -P /tmp/ $apr_util_location
		cd /tmp
		tar -zxvf ${apr_util_name}.tar.gz
		cd $apr_util_name
		./configure --with-apr=/usr/local/apr
		make
		make install
	fi

	if [ ! -f /etc/ld.so.conf.d/apr.conf ]; then
		echo "/usr/local/apr/lib" >/etc/ld.so.conf.d/apr.conf
	fi
    
	cd $cur_dir

	ldconfig
}

#install dpdk

function install_dpdk(){
    echo 'install dpdk----------------------------------'
    cp dpdk-${dpdk_ver}.tar.gz build
    cd build
    tar -zxvf dpdk-${dpdk_ver}.tar.gz
    cd dpdk-${dpdk_ver}
    make install T=x86_64-native-linuxapp-gcc
    cp -r x86_64-native-linuxapp-gcc/* $dpdk_install_prefix
    cp -r tools $dpdk_install_prefix
    rm -rf $dpdk_install_prefix/build
    cd ../../
}

function install_cloudhands(){
    
     export RTE_SDK=`pwd`/build/dpdk-${dpdk_ver}
     cp -r core build
     cp -r util build
     cp -r assemble build
     cp -r app build
     cp -r test build

     cp Makefile build

     make -C build/
     cp build/core/MakefileLib build/core/Makefile
     make -C build/core

     make -C build/test/

     if [[ ! -d $dpdk_install_prefix/CloudHands ]];then
         mkdir $dpdk_install_prefix/CloudHands
     fi
     cp -r build/core/build/app/* $dpdk_install_prefix/CloudHands
     cp -r build/test/build/app/* $dpdk_install_prefix/CloudHands
     cp -r conf $dpdk_install_prefix/CloudHands
}

function prepare_run_env(){
    modprobe uio 
    insmod $dpdk_install_prefix/kmod/igb_uio.ko
}

install_apr
install_dpdk
install_cloudhands
prepare_run_env

