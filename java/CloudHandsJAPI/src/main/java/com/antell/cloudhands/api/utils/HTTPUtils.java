package com.antell.cloudhands.api.utils;

import com.antell.cloudhands.api.packet.ByteData;
import com.antell.cloudhands.api.packet.TCPSession;

public class HTTPUtils {

    public static final boolean isHttpSession(String text, boolean isReq) {

        if (TextUtils.isEmpty(text))
            return false;

        if (isReq) {
            return text.contains("HTTP/");
        } else {

            return text.startsWith("HTTP/");
        }
    }

    public static final boolean isHttpSession(ByteData byteData, boolean isReq) {

        return isHttpSession(byteData.decode(), isReq);
    }

    public static final boolean isHTTPSession(TCPSession tcpSession) {

        return isHttpSession(tcpSession.getReqContent(), true) || isHttpSession(tcpSession.getResContent(), false);
    }

}
