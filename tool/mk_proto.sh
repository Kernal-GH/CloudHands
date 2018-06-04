#!/bin/bash
#
#      Filename: mk_proto.sh
#
#        Author: csp001314@163.com
#   Description: ---
#        Create: 2016-11-03 02:44:15
# Last Modified: 2016-11-03 02:58:09
#

rm -rf /tmp/build
mkdir -p /tmp/build

mkdir -p /tmp/proto/test/

cp -r ../util /tmp/build/
cp -r ../protocols /tmp/build/
cp ../make.include /tmp/build

cp -r /tmp/build/util/MakefileNDPDK /tmp/build/util/Makefile
make -C  /tmp/build/util/
make -C  /tmp/build/protocols/tproto
make -C  /tmp/build/protocols/tproto/http
make -C  /tmp/build/protocols/tproto/mail

make -C  /tmp/build/protocols/uproto
make -C  /tmp/build/protocols/uproto/dns

make -C  /tmp/build/protocols/

cp -r /tmp/build/protocols/proto_main /tmp/proto/test/
cp -r /tmp/build/protocols/conf /tmp/proto/test/

cd /tmp/proto/test
