package com.antell.cloudhands.api.packet.tcp.http;

public class ContentRange {

    private final long from;
    private final long to;
    private final long totalLength;

    private static final int getDigitIndex(String contentRange){

         for(int i = 0;i<contentRange.length();i++){

            if(contentRange.charAt(i)>='0'&&contentRange.charAt(i)<='9')
                return i;;
        }

        return -1;
    }

    public long getTotalLength() {
        return totalLength;
    }

    public long getFrom() {
        return from;
    }

    public long getTo() {
        return to;
    }

    public ContentRange(String contentRange) throws InvalidContentRangeException{

        int startIndex = getDigitIndex(contentRange);

        if(startIndex == -1)
            throw new InvalidContentRangeException(contentRange);

        int endIndex = contentRange.indexOf("-");

        if(startIndex>=endIndex)
            throw new InvalidContentRangeException(contentRange);

        from = Long.parseLong(contentRange.substring(startIndex,endIndex));
        to = Long.parseLong(contentRange.substring(endIndex+1,contentRange.indexOf("/")));
        totalLength = Long.parseLong(contentRange.substring(contentRange.indexOf("/")+1));

    }

    public ContentRange(HTTPSession httpSession) throws InvalidContentRangeException {

        this(httpSession.getContentRange());
    }

    public String toString(){

        return "from:"+from+",to:"+to+",total:"+totalLength;
    }


}
