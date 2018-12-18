## CloudHands
``` 
利用DPDK捕获流量，流量一般从交换机镜像一个端口并连接到设备工作网口。从捕获的流量进行重组，统计等操作。
并对TCP，UDP等为基础的上层应用协议进行还原为进一步的安全分析提供必要的数据。 



```

### 依赖软件
```
1. msgpack
   wget https://github.com/shajf/msgpack-c/archive/master.zip
   unzip master.zip
   cd msgpack-c-master
   ./bootstrap
   ./configure
   make 
   make install
   
2. java8,maven
  cloudhands 提供了java api用来读取底层协议还原的结果及其一些流量元数据，以便用来
  进一步的业务处理，比如安全分析，流量分析等等，笔者强烈建议使用者最好使用java或是
  python等更高层的语言来处理业务相关的问题。另外还提供了java api 使用的sample.
  所以需要安装java8和maven
  
3. dpdk版本的选择
   目前cloudhands支持内置了dpdk的版本：
   17.05.2
   17.08.2
   18.02.1
   通过修改build/dpdk.sh 脚本来选择自己的dpdk版本，不同的操作系统版本酌情选择相关的版本，
   不然编译可能通不过
   ```
   
### 编译方法
```
1. cd build
2. sh make.sh
3. ./CloudHands_1.0.0.bin
等待编译和安装，完成之后，将其安装在目录 /usr/local/dpdk,目录结构如下：
/usr/local/dpdk
--------------/app        #dpdk 编译产生的自带应用程序安装目录
--------------/build      #dpdk 编译相关文件存储目录
--------------/include    #dpdk 编译产生的头文件安装目录
--------------/kmod       #dpdk 编译产生的内核模块安装目录
--------------/lib        #dpdk 编译产生的库文件安装目录
--------------/usertools  #dpdk 操作dpdk相关脚本的安装目录
--------------/dpdk_src   #指向dpdk源代码的软连接
--------------/CloudHands #本应用编译结果安装目录
--------------/CloudHands/bin    #CloudHands 可执行文件安装目录
--------------/CloudHands/conf   #CloudHands 配置文件安装目录
--------------/CloudHands/lib    #CloudHands 库文件安装目录
--------------/CloudHands/sample #CloudHands 使用例子安装目录

```
### 运行方法
```
1. cd /usr/local/dpdk/CloudHands/bin
   修改setup_config 中要绑定dpdk网卡bus_info,和内核驱动名称
   通过ethtool -i <网卡名称> 获得相关信息
   比如：INTERFACE=0000:82:00.1
         DRIVER=igb
   另外主要程序要绑定的cpu 按照自己的cpu core 数来调整，
   多个程序可以绑定同一个cpu core,但是cpu 核数充足的情况下，最好单独绑定

2. ./ch_setup.sh start 启动
   ./ch_setup.sh stop  停止
   
   启动之后
   ps -ef |grep PDispatcher
   ps -ef |grep TCPMain
   ps -ef |grep UDPMain
   ps -ef |grep SAMain
   看看这几个进程是否起来，若都起来那么启动成功，若是不成功，则查看/opt/cloudhands/log 下面的对应日志
   
3. 运行例子
   cd /usr/local/dpdk/CloudHands/sample
   ./jsample

4.查看流量统计信息
cd /usr/local/dpdk/CloudHands/bin
./StatDump /opt/cloudhands/data/store/sa_stat.data d 10
除了总流量，总包数外，对tcp,udp,icmp等协议做了统计，统计时间间隔为5分钟，每一个统计值（包数，字节数）
```
### 基于TCP的协议还原
```
1. HTTP
2. SMTP
3. POP3
4. IMAP
5. FTP
6. TELNET
7. mysql
8. redis
9. mongodb
10. ssh

```
### 基于UDP的协议还原
```
1. DNS
2. tftp
3. snmp

```
### 流量分析相关的元数据


### 作者信息
```
shajf,从事网络安全相关工作，擅长相关产品设计和开发工作

```
![微信二维码](image/wxin.png)
```
邮箱：csp001314@163.com
有问题欢迎随时联系

```


