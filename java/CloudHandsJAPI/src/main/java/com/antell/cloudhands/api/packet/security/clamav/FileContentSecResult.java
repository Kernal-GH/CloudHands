package com.antell.cloudhands.api.packet.security.clamav;

import com.antell.cloudhands.api.packet.tcp.FileTranSession;
import com.antell.cloudhands.api.source.SourceEntry;
import com.antell.cloudhands.api.utils.TextUtils;
import org.elasticsearch.common.xcontent.XContentBuilder;

import java.io.IOException;

public class FileContentSecResult implements SourceEntry {

    private final String objectId;
    private final FileTranSession fileTranSession;

    private final String virusType;

    public FileContentSecResult(FileTranSession fileTranSession, String virusType) {
        this.objectId = TextUtils.getUUID();
        this.fileTranSession = fileTranSession;
        this.virusType = virusType;
    }

    public FileTranSession getFileTranSession() {
        return fileTranSession;
    }

    public String getVirusType() {
        return virusType;
    }

    @Override
    public String dataToString() {

        StringBuffer sb = new StringBuffer();
        TextUtils.addText(sb,"file",fileTranSession.dataToString());
        TextUtils.addText(sb,"virusType",virusType);

        return sb.toString();
    }

    public String toString(){
        return dataToString();
    }

    @Override
    public XContentBuilder dataToJson(XContentBuilder cb) throws IOException {

        fileTranSession.dataToJson(cb);
        cb.field("objectId",objectId);
        cb.field("virusType",virusType);

        return cb;
    }

    public String getObjectId() {
        return objectId;
    }
}
