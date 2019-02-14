#!/bin/bash
# author shajf
# date 2016-03-03
##get path####

self_path=$(cd $(dirname "$0"); pwd)

############# product version #############
if [ -z $1 ]
then
    PRODUCT_VERSION="1.0.0"
else
    PRODUCT_VERSION=$1
fi

product_name=CloudHands_$PRODUCT_VERSION.bin

########## create dir ########
if [ -f $product_name ]
then
    rm -rf $product_name
fi


if [ ! -d build_dir ]; then
    mkdir build_dir
else
    rm -rf build_dir/*
fi

tar_path=./build_dir/package/tar
src_path=./build_dir/package/CloudHands

mkdir -p $tar_path
mkdir -p $src_path

. $self_path/dpdk.sh

echo $dpdk_ver

if [ ! -f $tar_path/dpdk-${dpdk_ver}.tar.gz ] ; then
	cp -r ../DPDK .
	cd DPDK
	sh make_dpdk_package.sh $dpdk_ver
	cd ../
	cp DPDK/build/* $tar_path
	rm -rf DPDK
fi

#################### cp files ##########################
echo "Begin copy files......"
cd $self_path

cp -rf ./makeself ./build_dir/

cp -rf ../make.include $src_path
cp -r ../lib $src_path
cp -r ../PDispatcher $src_path
cp -r ../conf $src_path
cp -r ../packet $src_path
cp -r ../MProcess $src_path
cp -r ../tcp $src_path
cp -r ../udp $src_path
cp -r ../StreamAnalyze $src_path
cp -r ../java $src_path
cp -r ../sample $src_path
cp -r ../bin $src_path
cp -r ../SessionMonitor $src_path
cp -r ../WBList $src_path
cp -r ../statistic $src_path

cp -f ./*.sh ./build_dir/

###### build file ##############
echo 'Begin to build installation Package......'
cd $self_path/build_dir
cp -f ./*.sh ./package/
chmod +x ./package/*.sh
chmod +x $self_path/build_dir/makeself/*.sh
./makeself/makeself.sh --gzip package $product_name 'Product Install' ./install.sh

cd $self_path
mv build_dir/$product_name .
rm -rf build_dir
echo "Build finish!!!!!"
