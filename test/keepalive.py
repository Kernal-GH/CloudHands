import socket
import time
import threading,thread
import sys
# 2013_4_25 by yinbin

def do_send(sc,reqstr):

    sc.send(reqstr)

    while True:
        recv = sc.recv(160000)
        if recv is None:
            break
        print(len(recv),type(recv))
        fd = open("chunk1.data","w+")
        fd.write(recv)

    fd.close()

if __name__ == "__main__":
    if len(sys.argv)<3:
        print("Usage:<ip> <urlpath>")
        exit(-1)

    ip = sys.argv[1]
    path = sys.argv[2]

    fd = open(path)
    urls = fd.readlines()
    fd.close()

    sc = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
    sc.setsockopt(socket.SOL_SOCKET,socket.SO_REUSEADDR,1)
    #sc.setsockopt(socket.SOL_SOCKET,socket.SO_KEEPALIVE,1)
    try:
        sc.connect((ip,80))
    except socket.error, e:
        print "server is down ",e
        sys.exit()
    for item in urls:
        url = item[:len(item)-1]
        reqstr="GET %s HTTP/1.1\r\nAccept: */*\r\nAccept-Language: zh-cn\r\nAccept-Encoding: gzip, deflate\r\nHost: www.xjishu.com\r\nConnection: Keep-Alive\r\n\r\n" %(url)
        do_send(sc,reqstr)

    time.sleep(10)
    sc.close()


