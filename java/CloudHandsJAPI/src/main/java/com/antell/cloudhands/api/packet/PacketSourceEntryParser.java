package com.antell.cloudhands.api.packet;

import com.antell.cloudhands.api.packet.smon.SMonSession;
import com.antell.cloudhands.api.packet.tcp.SessionParser;
import com.antell.cloudhands.api.packet.tcp.ftp.FTPDataSessionParser;
import com.antell.cloudhands.api.packet.tcp.ftp.FTPSession;
import com.antell.cloudhands.api.packet.tcp.http.BasicHttpPartContentParser;
import com.antell.cloudhands.api.packet.tcp.http.HTTPSessionParser;
import com.antell.cloudhands.api.packet.tcp.mail.MailSessionParser;
import com.antell.cloudhands.api.packet.tcp.ssh.SSHSession;
import com.antell.cloudhands.api.packet.tcp.telnet.TelnetSession;
import com.antell.cloudhands.api.packet.udp.dns.DNSSession;
import com.antell.cloudhands.api.packet.udp.tftp.BasicTFTPSessionParser;
import com.antell.cloudhands.api.packet.udp.tftp.TFTPSessionParser;
import com.antell.cloudhands.api.source.SourceEntry;
import com.antell.cloudhands.api.source.SourceEntryParser;

import java.io.IOException;
import java.util.Arrays;
import java.util.List;

/**
 * Created by dell on 2018/6/19.
 */
public class PacketSourceEntryParser implements SourceEntryParser {

    private final SessionParser httpSessionParser;
    private final SessionParser mailSessionParser;
    private final SessionParser ftpDataSessionParser;

    private final TFTPSessionParser tftpSessionParser;

    public PacketSourceEntryParser() {

        httpSessionParser = new HTTPSessionParser(new BasicHttpPartContentParser());
        mailSessionParser = new MailSessionParser();
        ftpDataSessionParser = new FTPDataSessionParser();

        tftpSessionParser = new BasicTFTPSessionParser();

    }

    @Override
    public List<SourceEntry> parse(PacketRecord packetRecord) throws IOException {

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
                entry = new DNSSession(packetRecord.getMessageUnpacker());
                break;

            case PacketRecord.TFTP:
                return tftpSessionParser.parse(packetRecord.getMessageUnpacker());

            case PacketRecord.HTTP:
            case PacketRecord.SECRESHTTP:
                return httpSessionParser.parse(packetRecord.getMessageUnpacker());

            case PacketRecord.FTP:
                entry = new FTPSession(packetRecord.getMessageUnpacker());
                break;

            case PacketRecord.FTPDATA:
                return ftpDataSessionParser.parse(packetRecord.getMessageUnpacker());

            case PacketRecord.MAIL:
            case PacketRecord.SECRESMAIL:
                return mailSessionParser.parse(packetRecord.getMessageUnpacker());

            case PacketRecord.TELNET:
                entry = new TelnetSession(packetRecord.getMessageUnpacker());
                break;

            case PacketRecord.TCPSMON:
                entry = new SMonSession(packetRecord.getMessageUnpacker(),true);
                break;

            case PacketRecord.UDPSMON:
                entry = new SMonSession(packetRecord.getMessageUnpacker(),false);
                break;

            case PacketRecord.SSH:
                entry = new SSHSession(packetRecord.getMessageUnpacker());
                break;

            default:
                entry = null;
                break;
        }

        return Arrays.asList(entry);
    }


}
