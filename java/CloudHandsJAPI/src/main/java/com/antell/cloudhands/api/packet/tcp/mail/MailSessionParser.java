package com.antell.cloudhands.api.packet.tcp.mail;

import com.antell.cloudhands.api.source.SourceEntry;
import org.msgpack.core.MessageUnpacker;

import java.util.List;

public interface MailSessionParser {

    List<SourceEntry> parse(MessageUnpacker unpacker);
}
