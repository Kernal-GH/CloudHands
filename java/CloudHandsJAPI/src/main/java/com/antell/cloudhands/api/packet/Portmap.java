package com.antell.cloudhands.api.packet;

import com.antell.cloudhands.api.utils.HTTPUtils;

import java.util.HashMap;
import java.util.Map;

/**
 * Created by dell on 2017/6/28.
 */
public class Portmap {

    private final static Map<Integer,PortItem> ports = new HashMap<>();

    static {

        ports.put(7,new PortItem(7,"echo","echo","应答协议"));
        ports.put(19,new PortItem(19,"chargen","chargen","字符发生器协议"));
        ports.put(20,new PortItem(20,"ftp","ftp.data","FTP传输数据"));
        ports.put(21,new PortItem(21,"ftp","ftp.cmd","FTP传输命令"));
        ports.put(22,new PortItem(22,"ssh","ssh","安全外壳协议"));
        ports.put(23,new PortItem(23,"telnet","telnet","远程终端协议"));
        ports.put(25,new PortItem(25,"mail","smtp","简单邮件传输协议"));
        ports.put(42,new PortItem(42,"winsReplication","winsReplication","WINS复制"));
        ports.put(49,new PortItem(49,"tacacs","tacacs","终端访问控制器访问控制系统"));
        ports.put(53,new PortItem(53,"dns","dns","域名系统"));
        ports.put(67,new PortItem(67,"dhcp","dhcp.server","动态主机配置协议"));
        ports.put(68,new PortItem(68,"dhcp","dhcp.client","动态主机配置协议"));
        ports.put(69,new PortItem(69,"tftp","tftp","简单文件传送协议"));
        ports.put(70,new PortItem(70,"gopher","gopher","信息查找系统"));
        ports.put(79,new PortItem(79,"finger","finger","finger服务"));
        ports.put(80,new PortItem(80,"http","http","超文本传输协议"));
        ports.put(88,new PortItem(88,"kerberos","kerberos","网络认证协议"));
        ports.put(102,new PortItem(102,"msexchange","msexchange","MS Exchange"));
        ports.put(110,new PortItem(110,"mail","pop3","邮件协议版本3"));
        ports.put(113,new PortItem(113,"ident","ident","ident认证协议"));
        ports.put(119,new PortItem(119,"nntp","nntp","网络新闻传输协议"));
        ports.put(123,new PortItem(123,"ntp","ntp","网络时间协议"));
        ports.put(135,new PortItem(135,"msrpc","msrpc","Microsoft RPC"));
        ports.put(137,new PortItem(137,"netbios","netbios","网络基本输入/输出系统协议"));
        ports.put(139,new PortItem(139,"netbios","netbios","网络基本输入/输出系统协议"));
        ports.put(143,new PortItem(143,"mail","imap","交互邮件访问协议"));
        ports.put(161,new PortItem(161,"snmp","snmp","简单网络管理协议"));
        ports.put(162,new PortItem(162,"snmp","snmp","简单网络管理协议"));
        ports.put(177,new PortItem(177,"xdmcp","xdmcp","X显示监控协议"));
        ports.put(179,new PortItem(179,"bgp","bgp","BGP路由协议"));
        ports.put(201,new PortItem(201,"appletalk","appletalk","AppleTalk协议"));
        ports.put(264,new PortItem(264,"bgmp","bgmp","边界网关组播协议"));
        ports.put(318,new PortItem(318,"tsp","tsp","时间戳协议"));
        ports.put(443,new PortItem(443,"https","https","超文本安全传输协议"));
        ports.put(389,new PortItem(389,"ldap","ldap","轻量目录访问协议"));
        ports.put(445,new PortItem(445,"microsoft-ds","microsoft-ds","共享文件夹或共享打印机"));
        ports.put(464,new PortItem(464,"kerberos","kerberos","网络认证协议"));
        ports.put(465,new PortItem(465,"mail","smtps","ssl简单邮件传输协议"));
        ports.put(497,new PortItem(497,"retrospect","retrospect","retrospect"));
        ports.put(500,new PortItem(500,"isakmp","isakmp","Internet密钥交换协议"));
        ports.put(512,new PortItem(512,"rexec","rexec","远程命令执行"));
        ports.put(513,new PortItem(513,"rlogin","rlogin","远程登录命令"));
        ports.put(514,new PortItem(514,"syslog","syslog","syslog"));
        ports.put(515,new PortItem(515,"lpd","lpd","行式打印机"));
        ports.put(520,new PortItem(520,"rip","rip","rip路由协议"));
        ports.put(521,new PortItem(521,"ripng","ripng","rip for ipv6"));
        ports.put(540,new PortItem(540,"uucp","uucp","UNIX至UNIX的拷贝"));
        ports.put(554,new PortItem(554,"rtsp","rtsp","实时流传输协议"));
        ports.put(560,new PortItem(560,"rmonitor","rmonitor","rmonitor"));
        ports.put(563,new PortItem(563,"nntps","nntps","安全网络新闻传输协议"));
        ports.put(587,new PortItem(587,"mail","smtp","简单邮件传输协议"));
        ports.put(591,new PortItem(591,"filemaker","filemaker","filemaker"));
        ports.put(593,new PortItem(593,"microsoft-dcom","microsoft-dcom","microsoft-dcom"));
        ports.put(631,new PortItem(631,"internet-printing","internet-print","网络打印机"));
        ports.put(1433,new PortItem(1433,"mssql","mssql","mssqlserver"));
        ports.put(3389,new PortItem(3389,"rdp","rdp","远程桌面登录协议"));
        ports.put(3306,new PortItem(3306,"mysql","mysql","MYSQL协议"));
        ports.put(6379,new PortItem(6379,"redis","redis","redis协议"));
        ports.put(8080,new PortItem(8080,"http","http","超文本传输协议"));

    }

    public static PortItem getPortItem(int srcPort,int dstPort){

        PortItem portItem = ports.get(srcPort);
        if(portItem!=null)
            return portItem;

        portItem = ports.get(dstPort);
        if(portItem!=null)
            return portItem;

        return new PortItem(0,"unknown","unknown","未知");
    }

    public static PortItem getPortItem(TCPSession tcpSession){

        PortItem portItem = getPortItem(tcpSession.getReqPort(),tcpSession.getResPort());
        if(portItem.getPort() != 0)
            return portItem;

        boolean isHttp = HTTPUtils.isHTTPSession(tcpSession);

        if(!isHttp)
            return portItem;

        return new PortItem(tcpSession.getResPort(),"http","http","超文本传输协议");

    }
}
