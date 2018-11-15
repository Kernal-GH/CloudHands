package com.antell.cloudhands.api;

import org.msgpack.core.MessageUnpacker;

import java.io.IOException;

/**
 * Created by dell on 2018/6/15.
 */
public interface MsgPackDataInput {

    void parse(MessageUnpacker unpacker) throws IOException;

}
