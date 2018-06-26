package com.antell.cloudhands.api.sample;

import com.antell.cloudhands.api.context.Context;
import com.antell.cloudhands.api.context.PropertyContext;
import com.antell.cloudhands.api.packet.*;
import com.antell.cloudhands.api.packet.tcp.http.HTTPSession;
import com.antell.cloudhands.api.packet.tcp.mail.MailSession;
import com.antell.cloudhands.api.packet.udp.dns.DNSSession;
import com.antell.cloudhands.api.source.Filter;
import com.antell.cloudhands.api.source.Source;
import com.antell.cloudhands.api.source.SourceEntry;

/**
 * Created by dell on 2018/6/19.
 */
public class Simple {

    /*过滤类，可以根据自己的需要选择要处理的流量，提高性能*/
    private static class SourceFilter implements Filter {

        /*选择http,mail,dns议议*/
        public boolean isAccept(PacketRecord packetRecord) {

            int type = packetRecord.getType();

            switch (type){

                case PacketRecord.HTTP:
                case PacketRecord.MAIL:
                case PacketRecord.DNS:
				case PacketRecord.ARP:
				case PacketRecord.ICMP:
                case PacketRecord.TCP:
                case PacketRecord.UDP:
                case PacketRecord.SECRESHTTP:
                case PacketRecord.SECRESMAIL:

                    return true;
                default:
                        return false;
            }
        }

        public boolean isAccept(SourceEntry sourceEntry) {
            return true;
        }
    }

    private static void httpSessionHandle(HTTPSession httpSession){

        System.out.println(httpSession);
    }

    private static void mailSessionHandle(MailSession mailSession){

        System.out.println(mailSession);
    }
    private static void dnsSessionHandle(DNSSession dnsSession){

        System.out.println(dnsSession);
    }

    private static void consume(Source source){

        while(true) {

            SourceEntry sourceEntry = source.take();

            if (sourceEntry != null) {
				
                if (sourceEntry instanceof HTTPSession) {

                    HTTPSession httpSession = (HTTPSession) sourceEntry;
                    httpSessionHandle(httpSession);

                } else if (sourceEntry instanceof MailSession) {

                    MailSession mailSession = (MailSession) sourceEntry;
                    mailSessionHandle(mailSession);

                } else if (sourceEntry instanceof DNSSession) {

                    DNSSession dnsSession = (DNSSession) sourceEntry;
                    dnsSessionHandle(dnsSession);
                } else if(sourceEntry instanceof ICMPPacket){
                    ICMPPacket icmpPacket = (ICMPPacket)sourceEntry;
                    icmpPacketHandle(icmpPacket);
                }else if(sourceEntry instanceof ARPPacket){
                    ARPPacket arpPacket = (ARPPacket)sourceEntry;
                    arpPacketHandle(arpPacket);
                }else if(sourceEntry instanceof TCPSession){
                    TCPSession tcpSession = (TCPSession)sourceEntry;
                    tcpSessionHandle(tcpSession);
                }else if(sourceEntry instanceof UDPSession){
                    UDPSession udpSession = (UDPSession)sourceEntry;
                    udpSessionHandle(udpSession);
                }
            }
        }
    }

    private static void udpSessionHandle(UDPSession udpSession) {

        System.out.println(udpSession);
    }

    private static void tcpSessionHandle(TCPSession tcpSession) {
        System.out.println(tcpSession);
    }

    private static void arpPacketHandle(ARPPacket arpPacket) {

        System.out.println(arpPacket);
    }

    private static void icmpPacketHandle(ICMPPacket icmpPacket) {

        System.out.println(icmpPacket);
    }

    public static void main(String[] args) throws Exception {

        Context context;

        if(args.length!=1){

            System.err.println("Usage:<appName> <config file name>");
            System.exit(-1);
        }

        String cfname = args[0];
        /*load config file*/
        try {
            context = new PropertyContext(cfname);
        } catch (Exception e) {
            System.err.println("load config file:"+cfname+" failed!");
            throw e;
        }

        /*create a source*/
        Source source = new PacketSource();
        source.init(context,new SourceFilter());

        /*start source to read */
        source.start();

        consume(source);

        
    }


}
