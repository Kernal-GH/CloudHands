package com.antell.cloudhands.api.sink.kafka;

import com.antell.cloudhands.api.packet.*;
import com.antell.cloudhands.api.packet.smon.SMonSession;
import com.antell.cloudhands.api.packet.tcp.ftp.FTPDataSession;
import com.antell.cloudhands.api.packet.tcp.ftp.FTPSession;
import com.antell.cloudhands.api.packet.tcp.http.HTTPSession;
import com.antell.cloudhands.api.packet.tcp.mail.MailSession;
import com.antell.cloudhands.api.packet.udp.dns.DNSSession;
import com.antell.cloudhands.api.packet.udp.tftp.TFTPSession;
import com.antell.cloudhands.api.source.SourceEntry;

import java.io.IOException;


public class KafkaPacketDataParser {

    public static final SourceEntry parse(PacketRecord packetRecord) throws IOException {

        SourceEntry entry = null;
        int type = packetRecord.getType();

        switch (type) {

            case PacketRecord.ARP:
                entry = new ARPPacket(packetRecord.getDataInput(), packetRecord.getTime());
                break;

            case PacketRecord.ICMP:
                entry = new ICMPPacket(packetRecord.getDataInput(), packetRecord.getTime());
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

            case PacketRecord.TFTP:
                entry = new TFTPSession(packetRecord.getDataInput());
                break;
            case PacketRecord.HTTP:
            case PacketRecord.SECRESHTTP:
                entry = new HTTPSession(packetRecord.getMessageUnpacker());
                break;

            case PacketRecord.FTP:
                entry = new FTPSession(packetRecord.getMessageUnpacker());
                break;

            case PacketRecord.FTPDATA:
                entry = new FTPDataSession(packetRecord.getMessageUnpacker());
                break;

            case PacketRecord.MAIL:
            case PacketRecord.SECRESMAIL:
                entry = new MailSession(packetRecord.getMessageUnpacker());
                break;

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

        return entry;
    }
}