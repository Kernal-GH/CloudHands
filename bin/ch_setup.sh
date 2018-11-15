#!/bin/bash
#
#      Filename: ch_setup.sh
#
#        Author: csp001314@163.com 
#   Description: ---
#        Create: 2016-08-07 20:23:00
# Last Modified: 2018-07-30 11:08:09
#


cd $(dirname ${BASH_SOURCE[0]})/../../
export RTE_SDK=$PWD
echo "------------------------------------------------------------------------------"
echo " RTE_SDK exported as $RTE_SDK"
echo "------------------------------------------------------------------------------"
. $RTE_SDK/CloudHands/bin/setup_config

HUGEPGSZ=`cat /proc/meminfo  | grep Hugepagesize | cut -d : -f 2 | tr -d ' '`


#check dpdk&cloudhands has been installed
if [[ ! -f $RTE_SDK/kmod/igb_uio.ko ]]; then
	echo "DPDK has not been installed,exit!"
	exit -2
fi

if [[ ! -d $RTE_SDK/CloudHands ]]; then
	echo "CloudHands has not been installed,exit!"
	exit -2
fi

prefare_dirs(){

	if [[ ! -d ${DATA_STORE_DIR} ]]; then
		echo "create data store dir:${DATA_STORE_DIR}"
		mkdir -p ${DATA_STORE_DIR}
	fi

	rm -rf ${DATA_SHM_DIR}
	mkdir -p ${DATA_SHM_DIR}
	rm -rf ${LOG_STORE_DIR}
	mkdir -p ${LOG_STORE_DIR}

}

#
# Creates hugepage filesystem.
#
create_mnt_huge()
{
	echo "Creating /mnt/huge and mounting as hugetlbfs"
	sudo mkdir -p /mnt/huge

	grep -s '/mnt/huge' /proc/mounts > /dev/null
	if [ $? -ne 0 ] ; then
		sudo mount -t hugetlbfs nodev /mnt/huge
	fi
}

#
# Removes hugepage filesystem.
#
remove_mnt_huge()
{
	echo "Unmounting /mnt/huge and removing directory"
	grep -s '/mnt/huge' /proc/mounts > /dev/null
	if [ $? -eq 0 ] ; then
		sudo umount /mnt/huge
	fi

	if [ -d /mnt/huge ] ; then
		sudo rm -R /mnt/huge
	fi
}

#
# Unloads igb_uio.ko.
#
remove_igb_uio_module()
{
	echo "Unloading any existing DPDK UIO module"
	/sbin/lsmod | grep -s igb_uio > /dev/null
	if [ $? -eq 0 ] ; then
		sudo /sbin/rmmod igb_uio
	fi
}

#
# Loads new igb_uio.ko (and uio module if needed).
#
load_igb_uio_module()
{
	if [ ! -f $RTE_SDK/kmod/igb_uio.ko ];then
		echo "## ERROR: Target does not have the DPDK UIO Kernel Module."
		echo "       To fix, please try to rebuild target."
		return
	fi

	remove_igb_uio_module

	/sbin/lsmod | grep -s uio > /dev/null
	if [ $? -ne 0 ] ; then
		modinfo uio > /dev/null
		if [ $? -eq 0 ]; then
			echo "Loading uio module"
			sudo /sbin/modprobe uio
		fi
	fi

	# UIO may be compiled into kernel, so it may not be an error if it can't
	# be loaded.

	echo "Loading DPDK UIO module"
	sudo /sbin/insmod $RTE_SDK/kmod/igb_uio.ko
	if [ $? -ne 0 ] ; then
		echo "## ERROR: Could not load kmod/igb_uio.ko."
		quit
	fi
}

#
# Removes all reserved hugepages.
#
clear_huge_pages()
{
	echo > .echo_tmp
	for d in /sys/devices/system/node/node? ; do
		echo "echo 0 > $d/hugepages/hugepages-${HUGEPGSZ}/nr_hugepages" >> .echo_tmp
	done
	echo "Removing currently reserved hugepages"
	sudo sh .echo_tmp
	rm -f .echo_tmp

	remove_mnt_huge
}

#
# Creates hugepages.
#
set_non_numa_pages()
{
	clear_huge_pages

	echo ""
	echo "  Input the number of ${HUGEPGSZ} hugepages"
	echo "  Example: to have 128MB of hugepages available in a 2MB huge page system,"
	echo "  enter '64' to reserve 64 * 2MB pages"
	echo "Number of pages: ${HUGE_PAGES}"

	echo "echo $HUGE_PAGES > /sys/kernel/mm/hugepages/hugepages-${HUGEPGSZ}/nr_hugepages" > .echo_tmp

	echo "Reserving hugepages"
	sudo sh .echo_tmp
	rm -f .echo_tmp

	create_mnt_huge
}

#
# Creates hugepages on specific NUMA nodes.
#
set_numa_pages()
{
	clear_huge_pages

	echo ""
	echo "  Input the number of ${HUGEPGSZ} hugepages for each node"
	echo "  Example: to have 128MB of hugepages available per node in a 2MB huge page system,"
	echo "  enter '64' to reserve 64 * 2MB pages on each node"

	echo > .echo_tmp
	for d in /sys/devices/system/node/node? ; do
		node=$(basename $d)
		echo -n "Number of pages for $node: "
		echo "echo $HUGE_PAGES > $d/hugepages/hugepages-${HUGEPGSZ}/nr_hugepages" >> .echo_tmp
	done
	echo "Reserving hugepages"
	sudo sh .echo_tmp
	rm -f .echo_tmp

	create_mnt_huge
}

#
# Uses dpdk-devbind.py to move devices to work with igb_uio
#
bind_nics_to_igb_uio()
{
	if [ -d /sys/module/igb_uio ]; then
		echo -n "Enter PCI address of device to bind to IGB UIO driver: "
		sudo ${RTE_SDK}/usertools/dpdk-devbind.py -b igb_uio $INTERFACE && echo "OK"
	else
		echo "# Please load the 'igb_uio' kernel module before querying or "
		echo "# adjusting NIC device bindings"
	fi
}

#
# Uses dpdk-devbind.py to move devices to work with kernel drivers again
#
unbind_nics()
{
	echo -n "Enter name of kernel driver to bind the device to: $DRIVERR"
	sudo ${RTE_SDK}/usertools/dpdk-devbind.py -b $DRIVER $INTERFACE && echo "OK"
}

#
#setup cloudhands 
#
setup_cloudhands(){

	prefare_dirs

	ulimit -n 1000000

	${RTE_SDK}/CloudHands/bin/${PDIS_MAIN_NAME} ${PDIS_CORE_BIND} ${PDIS_CONFIG_FPATH} 1>/dev/null 2>&1 &
	sleep 10

	${RTE_SDK}/CloudHands/bin/${TCP_MAIN_NAME} --proc-type secondary ${TCP_CORE_BIND} ${TCP_CONFIG_FPATH} 1>/dev/null 2>&1 &
	${RTE_SDK}/CloudHands/bin/${UDP_MAIN_NAME} --proc-type secondary ${UDP_CORE_BIND} ${UDP_CONFIG_FPATH} 1>/dev/null 2>&1 &
	${RTE_SDK}/CloudHands/bin/${SA_MAIN_NAME}  --proc-type secondary ${SA_CORE_BIND} ${SA_CONFIG_FPATH} 1>/dev/null 2>&1 &

}

start_dpdk(){

	echo "load huge pages -------------------------------------"
	if [[ $IS_NUNA==0 ]]; then
		
		set_non_numa_pages
	else
	
		set_numa_pages
	fi

	echo "load igb_uio module--------------------------------"
	load_igb_uio_module

	echo "bind network interface into dpdk igb_uio--------------------"
	bind_nics_to_igb_uio

}

stop_dpdk(){

	echo "clean up all huge pages ---------------------------"
	clear_huge_pages

	echo "unbind network interface ----------------------------"
	unbind_nics

	echo "unload igb_uio--------------------------------"
	remove_igb_uio_module
}

start_all(){

	start_dpdk

	echo "setup cloudhands application based dpdk----------------------"
	setup_cloudhands
}

stop_all(){

	stop_dpdk

	echo "stop cloudhands application -----------------"
	pkill -9 ${PDIS_MAIN_NAME}
	pkill -9 ${TCP_MAIN_NAME}
	pkill -9 ${UDP_MAIN_NAME}
	pkill -9 ${SA_MAIN_NAME}
	pkill -9 ${PP_MAIN_NAME}
}

# See how we were called.
case "$1" in
    start)
        start_all
        ;;

    stop)
        stop_all
        ;;

    restart)
        stop_all
        start_all
        ;;
    
	start_dpdk)
        start_dpdk
        ;;
	
    stop_dpdk)
        stop_dpdk
        ;;
    *)
        echo "Usage: $prog {start|stop|restart|start_dpdk|stop_dpdk}"
esac

