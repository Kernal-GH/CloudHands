package com.antell.cloudhands.api.packet.tcp.http;

import com.antell.cloudhands.api.utils.TextUtils;

import java.util.HashMap;
import java.util.Map;

/**
 * Created by dell on 2018/7/20.
 */
public final class HttpFileTransTypes {

    private HttpFileTransTypes(){

    }

    private static final Map<String,String> TransFileTypes = new HashMap<>();

    static {
       TransFileTypes.put("doc","application/vnd.ms-word");
       TransFileTypes.put( "xls","application/vnd.ms-excel");
       TransFileTypes.put("ppt","application/vnd.ms-powerpoint");
       TransFileTypes.put("pps","application/vnd.ms-powerpoint");
       TransFileTypes.put("pdf","application/pdf");
       TransFileTypes.put("odt","application/vnd.oasis.opendocument.text");
       TransFileTypes.put("gz","application/x-gzip");
       TransFileTypes.put("tgz","application/x-gzip");
       TransFileTypes.put("bz","application/x-bzip2");
       TransFileTypes.put("bz2","application/x-bzip2");
       TransFileTypes.put("tbz","application/x-bzip2");
       TransFileTypes.put("zip","application/zip");
       TransFileTypes.put("rar","application/x-rar");
       TransFileTypes.put("tar","application/x-tar");
       TransFileTypes.put("7z","application/x-7z-compressed7z");

      // TransFileTypes.put("txt","text/plain");
       TransFileTypes.put("rtf","text/rtf");
       TransFileTypes.put("rtfd","text/rtfd");
       TransFileTypes.put("py","text/x-python");
       TransFileTypes.put("rb","text/x-ruby");
       TransFileTypes.put("sh","text/x-shellscript");
       TransFileTypes.put("pl","text/x-perl");
       TransFileTypes.put("sql","text/x-sql");
       TransFileTypes.put("java","text/x-java-source");

       TransFileTypes.put("mp3","audio/mpeg");
       TransFileTypes.put("wav","audio/wav");
       TransFileTypes.put("wma","audio/x-ms-wma");
       TransFileTypes.put("avi","video/x-msvideo");
       TransFileTypes.put("dv","video/x-dv");
       TransFileTypes.put("mp4","video/mp4");
       TransFileTypes.put("mpeg","video/mpeg");
       TransFileTypes.put("mpg","video/mpeg");
       TransFileTypes.put("mov","video/quicktime");

       TransFileTypes.put("mkv","video/x-matroska");
    }

    public static final boolean isFileTrans(String extName){

       if(TextUtils.isEmpty(extName))
          return false;

       return TransFileTypes.containsKey(extName);
    }
}
