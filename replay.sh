#!/bin/bash

[ `id -u` -ne 0 ] && {
	echo "You must run this by root" >&2
	exit 1
}

if [ $# != 2 ] ; then
    echo "Usage:<if><pcap>"
fi

tcpreplay -i $1 -l 1 -M 10m $2

