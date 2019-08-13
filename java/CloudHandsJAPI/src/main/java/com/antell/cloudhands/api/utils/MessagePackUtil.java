package com.antell.cloudhands.api.utils;

import org.msgpack.core.MessageUnpacker;

import java.io.IOException;
import java.util.List;

/**
 * Created by dell on 2018/6/11.
 */
public class MessagePackUtil {

    public static String parseText(MessageUnpacker unpacker) throws IOException {
        unpacker.unpackString();
        return unpacker.unpackString();
    }

    public static String parseText2(MessageUnpacker unpacker) throws IOException {
        return unpacker.unpackString();
    }

    public static int parseByte(MessageUnpacker unpacker) throws IOException {
        unpacker.unpackString();
        return unpacker.unpackByte();
    }

    public static int parseShort(MessageUnpacker unpacker) throws IOException {
        unpacker.unpackString();
        return unpacker.unpackShort();
    }

    public static int parseInt(MessageUnpacker unpacker) throws IOException {
        unpacker.unpackString();
        return unpacker.unpackInt();
    }

    public static long parseLong(MessageUnpacker unpacker) throws IOException {

        unpacker.unpackString();
        return unpacker.unpackLong();
    }

    public static byte[] parseBin(MessageUnpacker unpacker) throws IOException {

        unpacker.unpackString();
        int len = unpacker.unpackBinaryHeader();
        byte[] data = new byte[len];

        unpacker.readPayload(data);

        return data;
    }

    public static int parseMapHeader(MessageUnpacker unpacker,boolean hasName) throws IOException {

        if(hasName)
            unpacker.unpackString();

        return unpacker.unpackMapHeader();
    }

    public static int parseArrayHeader(MessageUnpacker unpacker,boolean hasName) throws IOException {

        if(hasName)
            unpacker.unpackString();

        return unpacker.unpackArrayHeader();
    }

    public static void parseArrayString(MessageUnpacker unpacker,boolean hasName,List<String> arr) throws IOException {

        if(hasName)
            unpacker.unpackString();

        int n = unpacker.unpackArrayHeader();
        for(int i = 0;i<n;i++){

            arr.add(unpacker.unpackString());

        }
    }

    public static void parseArrayMap(MessageUnpacker unpacker,boolean hasName,List<String> arr) throws IOException {

        if(hasName)
            unpacker.unpackString();

        int n = unpacker.unpackMapHeader();
        for(int i = 0;i<n;i++){

            unpacker.unpackString();
            arr.add(unpacker.unpackString());
        }
    }

}
