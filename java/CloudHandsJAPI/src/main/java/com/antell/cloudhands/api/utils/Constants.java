package com.antell.cloudhands.api.utils;

/**
 * Created by dell on 2018/6/8.
 */
public interface Constants {

    public static final int CR = 13; // <US-ASCII CR, carriage return (13)>
    public static final int LF = 10; // <US-ASCII LF, linefeed (10)>
    public static final int SP = 32; // <US-ASCII SP, space (32)>
    public static final int HT = 9;  // <US-ASCII HT, horizontal-tab (9)>
    public static boolean isWhitespace(final char ch) {
        return ch == SP || ch == HT || ch == CR || ch == LF;
    }
    public static final String[] statDataType2String = {"statAll", "statTCP", "statUDP", "statICMP", "statARP", "statOTHER"};

    public static final String[] protoID2String = {"HTTP", "SMTP", "POP3", "IMAP", "DNS", "DEBUG", "TELNET", "FTP","SMON","SSH"};

    public static final String ESLOGDOCTYPE="esdatabase_doc";
}
