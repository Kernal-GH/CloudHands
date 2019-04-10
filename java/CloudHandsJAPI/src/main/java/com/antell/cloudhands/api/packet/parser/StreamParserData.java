package com.antell.cloudhands.api.packet.parser;

import org.elasticsearch.common.xcontent.XContentBuilder;

import java.io.IOException;

public interface StreamParserData {

    void toJson(XContentBuilder cb) throws IOException;

}
