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
#################### cp files ##########################
echo "Begin copy files......"
cd $self_path

dpdk_ver=17.02
apr_location=http://apache.fayea.com//apr/apr-1.5.2.tar.gz
apr_util_location=http://apache.fayea.com//apr/apr-util-1.5.4.tar.gz
msgpack_oname=master
msgpack_location=https://codeload.github.com/shajf/msgpack-c/zip/master
msgpack_name=msgpack-c-master
apr_name=apr-1.5.2
apr_util_name=apr-util-1.5.4
tar_path=./build_dir/package/tar
src_path=./build_dir/package/CloudHands

mkdir -p $tar_path
mkdir -p $src_path

cp -rf ./makeself ./build_dir/

if [ ! -f $tar_path/${apr_name}.tar.gz ]; then

   wget -P $tar_path $apr_location
fi

if [ ! -f $tar_path/${apr_util_name}.tar.gz ]; then
   wget -P $tar_path $apr_util_location
fi

if [ ! -f $tar_path/${msgpack_name}.zip ]; then

	wget -P $tar_path $msgpack_location
	mv $tar_path/$msgpack_oname $tar_path/${msgpack_name}.zip
fi

if [ ! -f $tar_path/dpdk-${dpdk_ver}.tar.gz ] ; then
	cp -r ../DPDK .
	cd DPDK
	sh make_dpdk_package.sh $dpdk_ver
	cd ../
	cp DPDK/build/* $tar_path
	rm -rf DPDK
fi

cp -r ../app $src_path
cp -r ../assemble $src_path
cp -r ../packets $src_path
cp -r ../bin $src_path
cp -r ../conf $src_path
cp -r ../core $src_path
cp -r ../tool $src_path
cp -r ../protocols $src_path
cp -r ../test $src_path
cp -r ../util $src_path
cp -r ../Makefile $src_path
cp -r ../make.include $src_path

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
