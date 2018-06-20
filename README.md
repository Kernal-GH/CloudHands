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

```

### 基于TCP的协议还原
```
1. HTTP
2. MAIL（smtp,pop3,imap）

```
### 基于UDP的协议还原
```
1. DNS

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


