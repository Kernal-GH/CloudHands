package com.antell.cloudhands.api.packet.security.event;

import com.antell.cloudhands.api.source.SourceEntry;

public interface SecurityEventChecker {

    boolean isCheck(SourceEntry sourceEntry);
    SecurityEvent check(SourceEntry sourceEntry);

}
