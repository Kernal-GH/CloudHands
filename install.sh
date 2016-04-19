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

#install dpdk

function install_dpdk(){
    echo 'install dpdk----------------------------------'
    cp dpdk-2.0.0.zip build
    cd build
    unzip dpdk-2.0.0.zip
    cp ../common_linuxapp dpdk-2.0.0/config
    cd dpdk-2.0.0

    make install T=x86_64-native-linuxapp-gcc
    cp -r x86_64-native-linuxapp-gcc/* $dpdk_install_prefix
    cp -r tools $dpdk_install_prefix
    rm -rf $dpdk_install_prefix/build
    cd ../../
}

function install_cloudhands(){
    
     export RTE_SDK=`pwd`/build/dpdk-2.0.0
     cp -r core build
     cp -r util build
     cp -r assemble build
     cp -r app build

     cp Makefile build

     make -C build/

     if [[ ! -d $dpdk_install_prefix/CloudHands ]];then
         mkdir $dpdk_install_prefix/CloudHands
     fi
     cp -r build/core/build/app/* $dpdk_install_prefix/CloudHands
}

function prepare_run_env(){
    modprobe uio 
    insmod $dpdk_install_prefix/kmod/igb_uio.ko
}

install_dpdk
install_cloudhands
prepare_run_env

