package com.antell.cloudhands.api.packet;

import com.antell.cloudhands.api.packet.smon.SMonSession;
import com.antell.cloudhands.api.packet.tcp.http.BasicHTTPSessionParser;
import com.antell.cloudhands.api.packet.tcp.http.BasicHttpPartContentParser;
import com.antell.cloudhands.api.packet.tcp.http.HTTPSession;
import com.antell.cloudhands.api.packet.tcp.http.HTTPSessionParser;
import com.antell.cloudhands.api.packet.tcp.mail.BasicMailSessionParser;
import com.antell.cloudhands.api.packet.tcp.mail.MailSession;
import com.antell.cloudhands.api.packet.tcp.mail.MailSessionParser;
import com.antell.cloudhands.api.packet.udp.dns.DNSSession;
import com.antell.cloudhands.api.source.SourceEntry;
import com.antell.cloudhands.api.source.SourceEntryParser;

import java.io.IOException;
import java.util.Arrays;
import java.util.List;

/**
 * Created by dell on 2018/6/19.
 */
public class PacketSourceEntryParser implements SourceEntryParser {

    private final HTTPSessionParser httpSessionParser;
    private final MailSessionParser mailSessionParser;

    public PacketSourceEntryParser(){

        httpSessionParser = new BasicHTTPSessionParser(new BasicHttpPartContentParser());
        mailSessionParser = new BasicMailSessionParser();
    }

    @Override
    public List<SourceEntry> parse(PacketRecord packetRecord) throws IOException {

        SourceEntry entry = null;
        int type = packetRecord.getType();

        switch (type){

            case PacketRecord.ARP:
                entry = new ARPPacket(packetRecord.getDataInput(),packetRecord.getTime());
                break;

            case PacketRecord.ICMP:
                entry = new ICMPPacket(packetRecord.getDataInput(),packetRecord.getTime());
                break;

            case PacketRecord.TCP:
                entry = new TCPSession(packetRecord.getDataInput());
                break;

            case PacketRecord.UDP:
                entry = new UDPSession(packetRecord.getDataInput());
                break;

            case PacketRecord.DNS:
                entry = new DNSSession(packetRecord.getDataInput());
                break;

            case PacketRecord.HTTP:
            case PacketRecord.SECRESHTTP:
                return  httpSessionParser.parse(packetRecord.getMessageUnpacker());

            case PacketRecord.MAIL:
            case PacketRecord.SECRESMAIL:
                return mailSessionParser.parse(packetRecord.getMessageUnpacker());

            case PacketRecord.TCPSMON:
                entry = new SMonSession(packetRecord.getMessageUnpacker());
                break;

            case PacketRecord.UDPSMON:
                entry = new SMonSession(packetRecord.getDataInput());
                break;

            default:
                entry = null;
                break;
        }

        return Arrays.asList(entry);
    }


}
