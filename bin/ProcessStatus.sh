#!/bin/bash

#memory uplevel 3G
MMUPLEVEL=3145728

TIMELIMIT=1800000

SHMCMD=/usr/local/dpdk/CloudHands/bin/SHMInfoDump
TCPMainSHMPath=/opt/data/cloudhands/shm/pstore_1_http
UDPMainSHMPath=/opt/data/cloudhands/shm/udp_1
SAMainSHMPath=/opt/data/cloudhands/shm/sa_1
BigEyeSHMPath1=/opt/data/cloudhands/shm/match_result_1
BigEyeSHMPath2=/opt/data/cloudhands/shm/match_result_2

TCPMainProcessID=`ps -e |grep TCPMain |awk '{print $1}'`
UDPMainProcessID=`ps -e |grep UDPMain |awk '{print $1}'`
SAMainProcessID=`ps -e |grep  SAMain |awk '{print $1}'`
BigEyeProcessID=`ps -e |grep  BigEye |awk '{print $1}'`

TCPMainProcessRMM=`ps -e -o 'rsz,args'|grep TCPMain|grep -v grep|awk '{print $1}'`
UDPMainProcessRMM=`ps -e -o 'rsz,args'|grep UDPMain|grep -v grep|awk '{print $1}'`
SAMainProcessRMM=`ps -e -o 'rsz,args'|grep SAMain|grep -v grep|awk '{print $1}'`
BigEyeProcessRMM=`ps -e -o 'rsz,args'|grep BigEye|grep -v grep|awk '{print $1}'`

TCPMainWriteLastTime=`$SHMCMD $TCPMainSHMPath 7 |awk '{split($0,a,":");print a[2]}'`
UDPMainWriteLastTime=`$SHMCMD $UDPMainSHMPath 7 |awk '{split($0,a,":");print a[2]}'`
SAMainWriteLastTime=`$SHMCMD $SAMainSHMPath 7 |awk '{split($0,a,":");print a[2]}'`
BigEyeWriteLastTime1=`$SHMCMD $BigEyeSHMPath1 7 |awk '{split($0,a,":");print a[2]}'`
BigEyeWriteLastTime2=`$SHMCMD $BigEyeSHMPath2 7 |awk '{split($0,a,":");print a[2]}'`

curTime=$[$(date +%s%N)/1000000]

#check TCPMain Process is running??
if [  $TCPMainProcessID ]; then

    if [ $TCPMainProcessRMM -gt $MMUPLEVEL ]; then
        echo "TCPMain obtains too large memory:$TCPMainProcessRMM,will be restart!"
        kill -9 $TCPMainProcessID
    fi

    if [ $TCPMainWriteLastTime -ne 0 ];then
        diff=$[$curTime-$TCPMainWriteLastTime]
        if [ $diff -gt $TIMELIMIT ];then
            echo "TCPMain did not write data for a long time:$diff,may been not work,will be restart!"
            kill -9 $TCPMainProcessID
        fi
    fi
fi

#check UDPMain Process is running??
if [  $UDPMainProcessID ]; then

    if [ $UDPMainProcessRMM -gt $MMUPLEVEL ]; then
        echo "UDPMain obtains too large memory:$UDPMainProcessRMM,will be restart!"
        kill -9 $UDPMainProcessID
    fi

    if [ $UDPMainWriteLastTime -ne 0 ];then
        diff=$[$curTime-$UDPMainWriteLastTime]
        if [ $diff -gt $TIMELIMIT ];then
            echo "UDPMain did not write data for a long time:$diff,may been not work,will be restart!"
            kill -9 $UDPMainProcessID
        fi
    fi
fi

#check SAMain Process is running??
if [  $SAMainProcessID ]; then

    if [ $SAMainProcessRMM -gt $MMUPLEVEL ]; then
        echo "SAMain obtains too large memory:$SAMainProcessRMM,will be restart!"
        kill -9 $SAMainProcessID
    fi

    if [ $SAMainWriteLastTime -ne 0 ];then
        diff=$[$curTime-$SAMainWriteLastTime]
        if [ $diff -gt $TIMELIMIT ];then
            echo "SAMain did not write data for a long time:$diff,may been not work,will be restart!"
            kill -9 $SAMainProcessID
        fi
    fi
fi

#check BigEye Process is running??
if [  $BigEyeProcessID ]; then

    if [ $BigEyeProcessRMM -gt $MMUPLEVEL ]; then
        echo "BigEye obtains too large memory:$BigEyeProcessRMM,will be restart!"
        kill -9 $BigEyeProcessID
    fi

    if [ $BigEyeWriteLastTime1 -ne 0 ];then
        diff=$[$curTime-$BigEyeWriteLastTime1]
        if [ $diff -gt $TIMELIMIT ];then
            echo "BigEye did not write data for a long time:$diff,may been not work,will be restart!"
            kill -9 $BigEyeProcessID
        fi
    fi
fi
