#!/bin/bash
#
#      Filename: install.sh
#
#        Author: gongyanlei@antell.com.cn
#   Description: ---
#        Create: 2017-09-08 16:55:12
# Last Modified: 2017-09-11 15:37:39
#

rm -fr build
mkdir build

mkdir -p build/tar

cp DPDK/dpdk-17.02.tar.gz build/tar/

cp -fr app build/
cp -fr assemble build/
cp -fr packets build/
cp -fr bin build/
cp -fr core build/
cp -fr conf build/
cp -fr tool build/
cp -fr util build/
cp -fr Makefile build/
cp -fr make.include build/

cp install.sh build/

chmod a+x build/*.sh

