package com.antell.cloudhands.api.packet.tcp.http;

import com.antell.cloudhands.api.packet.tcp.TCPSessionEntry;
import com.antell.cloudhands.api.source.SourceEntry;
import com.antell.cloudhands.api.utils.TextUtils;

import java.io.IOException;
import java.util.*;

public class BasicHttpPartContentParser implements HttpPartContentParser {

    public static final long TIMEOUT = 10*60;

    private final Map<String,PartContentValue> partContents;

    public List<PartContentValue> moveAllTimeout(){

        List<PartContentValue> res = new ArrayList<>();
        List<String> keys = new ArrayList<>();

        long curTime = System.currentTimeMillis()/1000;

        partContents.forEach((k,v)->{
            if(curTime-v.getLastTime()>TIMEOUT){
                res.add(v);
                keys.add(k);
            }
        });

        keys.forEach(e->partContents.remove(e));

        return res;
    }

    private  class PartContentValue{

        private final HTTPSession httpSession;
        private final FileStorage fileStorage;
        private final long totalLength;
        private  long offset;
        private  long lastTime;

        private PartContentValue(HTTPSession httpSession, long totalLength,long offset) throws IOException {
            this.httpSession = httpSession;
            this.totalLength = totalLength;
            this.offset = offset;
            this.lastTime = System.currentTimeMillis()/1000;

            fileStorage = new FileStorage(httpSession.getResBodyPath(),offset);
        }

        public boolean isComplete(){

            return fileStorage.size() == totalLength&&offset == 0;
        }

        private String contentRangeMake(){

            StringBuffer sb = new StringBuffer();
            sb.append("bytes ");
            sb.append(offset);
            sb.append("-");
            sb.append(totalLength-1);
            sb.append("/");
            sb.append(totalLength);

            return sb.toString();
        }

        public HTTPSession getFinalHttpSession(){

            httpSession.setContentRange(contentRangeMake());
            TCPSessionEntry sessionEntry = (TCPSessionEntry) httpSession.getSessionEntry();
            sessionEntry.setResPBytes(fileStorage.size());

            fileStorage.close();

            return httpSession;
        }

        public long getLastTime() {
            return lastTime;
        }

        public void updateLastTime(){
            this.lastTime = System.currentTimeMillis()/1000;
        }

        public HTTPSession getHttpSession(){
            return httpSession;
        }

        public long getTotalLength() {
            return totalLength;
        }

        public long getOffset() {
            return offset;
        }

        public void setOffset(long offset) {

            if(offset<this.offset)
                this.offset = offset;
        }

        public FileStorage getFileStorage() {
            return fileStorage;
        }
    };

    public BasicHttpPartContentParser() {

        partContents = new HashMap<>();

    }

    private PartContentValue findPCV(HTTPSession httpSession){

        String key = httpSession.get206Key();
        PartContentValue partContentValue = null;

        if(TextUtils.isEmpty(key))
            return null;

        return partContents.get(key);
    }

    private PartContentValue createPCV(HTTPSession httpSession,ContentRange contentRange){

        String key = httpSession.get206Key();
        PartContentValue partContentValue = null;

        if(TextUtils.isEmpty(key))
            return null;

        try {
            partContentValue = new PartContentValue(httpSession,contentRange.getTotalLength(),contentRange.getFrom());
            partContents.put(key,partContentValue);
        } catch (IOException e) {

            e.printStackTrace();
            return null;
        }

        return partContentValue;
    }

    public List<SourceEntry> doParse(HTTPSession httpSession) {

        PartContentValue partContentValue = null;
        ContentRange  contentRange= null;
        try {
            contentRange = new ContentRange(httpSession.getContentRange());
        } catch (Exception e) {
            e.printStackTrace();
            return Arrays.asList(httpSession);
        }

        partContentValue = findPCV(httpSession);

        if(partContentValue != null){
            try {
                partContentValue.updateLastTime();
                partContentValue.getFileStorage().write(httpSession.getResBodyPath(),contentRange.getFrom(),true);
            } catch (IOException e) {
                e.printStackTrace();
                return Arrays.asList(httpSession);
            }
        }else{
            partContentValue = createPCV(httpSession,contentRange);
        }

        if(partContentValue == null) {
            return Arrays.asList(httpSession);
        }
        List<SourceEntry> res = new ArrayList<>();

        if(partContentValue.isComplete()){

            res.add(partContentValue.getFinalHttpSession());
            partContents.remove(httpSession.get206Key());


        }
        return res;
    }

    private final static boolean listEmpty(List<SourceEntry> list){

        return list == null || list.isEmpty();
    }

    @Override
    public List<SourceEntry> parse(HTTPSession httpSession) {

        List<SourceEntry> res = new ArrayList<>();

        List<SourceEntry> parseRes = doParse(httpSession);

        List<SourceEntry> toutRes = clean();

        if(!listEmpty(parseRes))
            res.addAll(parseRes);

        if(!listEmpty(toutRes))
            res.addAll(toutRes);

        return parseRes;
    }

    @Override
    public List<SourceEntry> clean() {

        List<PartContentValue> timeOutPCV = moveAllTimeout();

        if(timeOutPCV!=null&&!timeOutPCV.isEmpty()){

            List<SourceEntry> res = new ArrayList<>();
            timeOutPCV.forEach(pcv->{

                HTTPSession httpSession = pcv.getFinalHttpSession();
                res.add(httpSession);

            });

            return res;
        }else{
            return null;
        }
    }

}
