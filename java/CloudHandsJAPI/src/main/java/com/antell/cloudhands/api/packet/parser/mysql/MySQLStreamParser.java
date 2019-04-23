package com.antell.cloudhands.api.packet.parser.mysql;

import com.antell.cloudhands.api.packet.ByteData;
import com.antell.cloudhands.api.packet.SessionEntry;
import com.antell.cloudhands.api.packet.parser.StreamParser;
import com.antell.cloudhands.api.packet.parser.StreamParserData;
import com.antell.cloudhands.api.packet.tcp.TCPSessionEntry;
import com.antell.cloudhands.api.utils.TextUtils;

import java.util.Base64;

public class MySQLStreamParser implements StreamParser {


    private class MYSQLPacket {

        private final int packetLen;
        private final int seq;
        private final int offset;

        public MYSQLPacket(ByteData content) {

            this(content.getData());

        }

        public MYSQLPacket(byte[] data) {

            this(data, 0);

        }

        public MYSQLPacket(byte[] data, int offset) {

            packetLen = (data[offset] & 0xff) | ((data[offset + 1] & 0xff) << 8) | ((data[offset + 2] & 0xff) << 16);
            seq = data[offset + 3] & 0xff;
            this.offset = offset;
        }

        public int getPacketLen() {
            return packetLen;
        }

        public int getSeq() {
            return seq;
        }

        public int getOffset() {
            return offset;
        }
    }


    private static final boolean isParseLen(ByteData content, MYSQLPacket mysqlPacket) {

        if (mysqlPacket.getPacketLen() <= 0)
            return false;

        return true;
    }

    private static final boolean isMYSQLLoginRequest(ByteData byteData, MYSQLPacket mysqlPacket) {

        if (mysqlPacket.getSeq() != 1)
            return false;

        byte[] data = byteData.getData();

        for (int i = 13; i <= 35; i++) {

            if (data[i] != 0)
                return false;
        }

        return true;
    }

    @Override
    public boolean parsable(SessionEntry sessionEntry) {

        if (sessionEntry.getResPort() != 3306)
            return false;

        ByteData reqBytes = sessionEntry.getReqContent();
        if (reqBytes == null || reqBytes.getData() == null || reqBytes.getDataSize() < 37)
            return false;

        MYSQLPacket mysqlPacket = new MYSQLPacket(reqBytes);

        if ((!isParseLen(reqBytes, mysqlPacket)) || (!isMYSQLLoginRequest(reqBytes, mysqlPacket)))
            return false;

        return true;
    }

    private static final String getNullString(byte[] data, int offset) {

        int len = 0;

        for (int i = offset; i < data.length; i++) {

            if (data[i] == 0)
                break;

            len++;
        }

        return new String(data, offset, len);
    }


    private MySQLStreamParserData parseLoginRequest(SessionEntry sessionEntry) {

        String user = null;

        try {
            user = getNullString(sessionEntry.getReqContent().getData(), 36);
        } catch (Exception e) {
            user = null;
        }

        if (TextUtils.isEmpty(user))
            return null;

        return new MySQLStreamParserData(user);
    }


    private static final int getNextPacketOffset(MYSQLPacket packet) {

        return packet.getOffset() + packet.getPacketLen() + 4;
    }

    private MYSQLPacket getNextPacket(MYSQLPacket packet, ByteData content) {

        int offset = getNextPacketOffset(packet);

        if (offset + 4 >= content.getDataSize())
            return null;

        return new MYSQLPacket(content.getData(), offset);
    }

    private void parseLoginResponse(SessionEntry sessionEntry, MySQLStreamParserData parserData) {

        ByteData content = sessionEntry.getResContent();
        if (content == null || content.getData() == null || content.getDataSize() < 4)
            return;

        MYSQLPacket packet = new MYSQLPacket(content);
        if (!isParseLen(content, packet))
            return;

        if (packet.getSeq() != 0)
            return;

        try {
            String version = getNullString(content.getData(), 5);
            parserData.setVersion(version);

        } catch (Exception e) {

            return;
        }

        packet = getNextPacket(packet, content);
        if (packet != null) {

            if (packet.getSeq() == 0) {
                parserData.setLoginStatus(MySQLStreamParserData.LOGIN_FAILED);
            } else if (packet.getSeq() >= 2 && packet.getOffset() + 4 <= content.getDataSize()) {

                int flag = (content.getData()[packet.getOffset() + 4])&0xff;
                if (flag == 0xff) {
                    parserData.setLoginStatus(MySQLStreamParserData.LOGIN_FAILED);
                } else if (flag == 0 || flag == 0xfe) {
                    parserData.setLoginStatus(MySQLStreamParserData.LOGIN_OK);
                }
            }
        }
    }

    @Override
    public StreamParserData parse(SessionEntry sessionEntry) {

        MySQLStreamParserData parserData = parseLoginRequest(sessionEntry);
        if (parserData == null)
            return null;

        parseLoginResponse(sessionEntry, parserData);
        return parserData;
    }

    public static void main(String[] args){


        String req = "SQAAAY2iPgAAAABACAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAYWRtaW5pc3RyYXRvcgAUCplIeo3oBSE60PfTbEysUmpqZFpteXNxbAA=";
        String res = "NAAAAAo1LjEuNzMAij8RADFOdSVTUydmAP/3CAIAAAAAAAAAAAAAAAAAAFE/KEdwfEJvLTBIWQBVAAAC/xUEIzI4MDAwQWNjZXNzIGRlbmllZCBmb3IgdXNlciAnYWRtaW5pc3RyYXRvcidAJzYxLjQ4LjEzMS4yMzgnICh1c2k=";
        TCPSessionEntry sessionEntry = new TCPSessionEntry();
        sessionEntry.setResPort(3306);
        sessionEntry.setReqContent(new ByteData(Base64.getDecoder().decode(req)));
        sessionEntry.setResContent(new ByteData(Base64.getDecoder().decode(res)));

        MySQLStreamParser parser = new MySQLStreamParser();

        MySQLStreamParserData parserData = (MySQLStreamParserData) parser.parse(sessionEntry);
        boolean t = parser.parsable(sessionEntry);



    }

}
