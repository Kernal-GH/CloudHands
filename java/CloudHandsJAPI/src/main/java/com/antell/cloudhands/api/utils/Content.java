package com.antell.cloudhands.api.utils;

import java.io.*;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.HashMap;
import java.util.Map;
import java.util.zip.GZIPInputStream;
import java.util.zip.InflaterInputStream;

/**
 * Created by dell on 2017/5/23.
 */
public final class Content {

    private final static Map<String,String> contentType2ExtNames = new HashMap<>();
    private final static Map<String,String> extName2ContentType = new HashMap<>();
    static {

        contentType2ExtNames.put("application/postscript","ai");
        contentType2ExtNames.put("application/postscript","eps");
        contentType2ExtNames.put("application/vnd.ms-word","doc");
        contentType2ExtNames.put("application/vnd.ms-excel","xls");
        contentType2ExtNames.put("application/vnd.ms-powerpoint","ppt");
        contentType2ExtNames.put("application/vnd.ms-powerpoint","pps");
        contentType2ExtNames.put("application/pdf","pdf");
        contentType2ExtNames.put("application/xml","xml");
        contentType2ExtNames.put("application/vnd.oasis.opendocument.text","odt");
        contentType2ExtNames.put("application/x-shockwave-flash","swf");
        contentType2ExtNames.put("application/x-gzip","gz");
        contentType2ExtNames.put("application/x-gzip" ,"tgz");
        contentType2ExtNames.put("application/x-bzip2" ,"bz");
        contentType2ExtNames.put("application/x-bzip2" ,"bz2");
        contentType2ExtNames.put("application/x-bzip2" ,"tbz");
        contentType2ExtNames.put("application/zip","zip");
        contentType2ExtNames.put("application/x-rar" ,"rar");
        contentType2ExtNames.put("application/x-tar" ,"tar");
        contentType2ExtNames.put("application/x-7z-compressed7z","7z");

        contentType2ExtNames.put("text/json","json");
        contentType2ExtNames.put("text/plain","txt");
        contentType2ExtNames.put("text/x-php","php");
        contentType2ExtNames.put("text/html","html");
        contentType2ExtNames.put("text/javascript","js");
        contentType2ExtNames.put("text/css","css");
        contentType2ExtNames.put("text/rtf","rtf");
        contentType2ExtNames.put("text/rtfd","rtfd");
        contentType2ExtNames.put("text/x-python","py");
        contentType2ExtNames.put("text/x-ruby","rb");
        contentType2ExtNames.put("text/x-shellscript","sh");
        contentType2ExtNames.put("text/x-perl","pl");
        contentType2ExtNames.put("text/x-sql","sql");
        contentType2ExtNames.put("text/x-java-source","java");

         contentType2ExtNames.put("image/x-ms-bmp","bmp");
         contentType2ExtNames.put("image/jpeg","jpg");
         contentType2ExtNames.put("image/jpeg","jpeg");
         contentType2ExtNames.put("image/gif","gif");
         contentType2ExtNames.put("image/png","png");
         contentType2ExtNames.put("image/tiff","tif");
         contentType2ExtNames.put("image/tiff","tiff");
         contentType2ExtNames.put("image/x-targa","tga");
         contentType2ExtNames.put("image/vnd.adobe.photoshop","psd");

          contentType2ExtNames.put("audio/mpeg","mp3");
          contentType2ExtNames.put("audio/midi","mid");
          contentType2ExtNames.put("audio/ogg","ogg");
          contentType2ExtNames.put("audio/mp4","mp4a");
          contentType2ExtNames.put("audio/wav","wav");
          contentType2ExtNames.put("audio/x-ms-wma","wma");
          contentType2ExtNames.put("video/x-msvideo","avi");
          contentType2ExtNames.put("video/x-dv","dv");
          contentType2ExtNames.put("video/mp4","mp4");
          contentType2ExtNames.put("video/mpeg","mpeg");
          contentType2ExtNames.put("video/mpeg","mpg");
          contentType2ExtNames.put("video/quicktime","mov");
          contentType2ExtNames.put("video/x-ms-wmv","wm");
          contentType2ExtNames.put("video/x-flv","flv");
          contentType2ExtNames.put("video/x-matroska","mkv");

          for(Map.Entry<String,String> entry:contentType2ExtNames.entrySet()){

              extName2ContentType.put(entry.getValue(),entry.getKey());
          }

    };

    public static boolean isLow2Value(String path,long v){

        try {
            long fs = Files.size(Paths.get(path));
            return fs<=v;
        } catch (IOException e) {
            return false;
        }
    }

    public final static String getExtNameByContentType(String contentType){

        String extName = null;
        if(TextUtils.isEmpty(contentType))
            return null;

        extName = contentType2ExtNames.get(contentType);
        if(extName!=null)
            return extName;

        for(Map.Entry<String,String> entry:contentType2ExtNames.entrySet()){

            String k = entry.getKey();
            if(contentType.toLowerCase().contains(k))
                return entry.getValue();
        }

        return null;

    }

    public static String guessContentTypeFromUri(String uri){

        String extName = getExtName(null,uri);
        if(TextUtils.isEmpty(extName))
            return null;

        return extName2ContentType.get(extName);
    }

    public static String getExtName(String contentType,String uri){

        String extName = getExtNameByContentType(contentType);
        if(extName!=null)
            return extName;

        /*find in url path*/
        if(!TextUtils.isEmpty(uri)){

            String uriPath = URLUtils.getPath(uri);
            if(!TextUtils.isEmpty(uriPath)) {

                int index = uriPath.lastIndexOf(".");
                if (index != -1)
                    return uriPath.substring(index + 1);
            }
        }

        return "data";
    }

    public static final String getExtName(String fileName){

        if(TextUtils.isEmpty(fileName))
            return "data";

        int index = fileName.lastIndexOf(".");
        if(index<0)
            return "data";

        return fileName.substring(index+1);
    }

    public static final String getFileName(String contentType,String uri,String path){

        if(!TextUtils.isEmpty(uri)){

            String uriPath = URLUtils.getPath(uri);
            if(!TextUtils.isEmpty(uriPath)&&uriPath.lastIndexOf('.')!=-1){

                return uriPath.substring(uriPath.lastIndexOf("/")+1);
            }
        }
        String fName = path.substring(path.lastIndexOf("/")+1);
        String extName = getExtName(contentType,null);
        return extName == null?fName:fName+"."+extName;
    }

    public static final String getFileName(String contentDisposition){

        try {

            return contentDisposition.split(";")[1].split("=")[1].replaceAll("\"","");
        }catch (Exception e){
            return null;
        }

    }

    private static final boolean isText(String nct){

             if(nct.contains("text")||
                    nct.contains("htm")||
                    nct.contains("xml")||
                    nct.contains("json")||
                    nct.contains("css")||
                    nct.contains("java")||
                    nct.contains("jsp")||
                    nct.contains("py")){

                return true;
            }

            return false;
    }

    public static final boolean isBaseText(String contentType,String uri,String path) throws IOException {

        if (!TextUtils.isEmpty(contentType)) {

            boolean res = isText(contentType);
            if(res)
                return true;

        }

        String ext = getExtName(contentType,uri);
        if(!TextUtils.isEmpty(ext))
            return isText(ext);

        return false;
    }

    public static final boolean isBaseText(String contentType,String path) throws IOException {

        if(isBaseText(contentType,null,path))
            return true;

        byte[] data = new byte[128];
        RandomAccessFile af = new RandomAccessFile(path,"r");
        int rsize = af.read(data);

        int c = 0;
        for(int i = 0;i<rsize;i++){

            int ch = data[i];
            if(ch>=0&&ch<=256)
                c++;
        }

        af.close();
        return c>=rsize/2;
    }

    private static byte[] doUnCompressed(boolean gzip,byte[] data){
        if(data == null || data.length == 0)
            return null;

        ByteArrayOutputStream out = new ByteArrayOutputStream();
        ByteArrayInputStream in = new ByteArrayInputStream(data);
        try {
            InputStream zip;
            if(gzip)
                zip = new GZIPInputStream(in);
            else
                zip = new InflaterInputStream(in);

            byte[] buffer = new byte[data.length];
            int n;
            while ((n = zip.read(buffer))>= 0) {
                out.write(buffer, 0, n);
            }

            return out.toByteArray();

        } catch (IOException e) {
            return  null;
        }
    }

    private static boolean hasCompressed(String compressType) {

        if(TextUtils.isEmpty(compressType))
            return false;

        String ecv = compressType.toLowerCase();
        return ecv.contains("gzip")|| ecv.contains("deflate");
    }

    private static boolean isGzip(String compressType){
        if(TextUtils.isEmpty(compressType))
            return false;

        String ecv = compressType.toLowerCase();
        return ecv.contains("gzip");
    }

    private static String getText(byte[] content){
        try {
            return new String(content,"utf-8");
        } catch (UnsupportedEncodingException e) {

            return new String(content);
        }
    }

    public static String readBodyAsText(String path,String compressType){

        try {
            byte[] data = Files.readAllBytes(Paths.get(path));
            if(hasCompressed(compressType)){
                byte[] udata = doUnCompressed(isGzip(compressType),data);
                if(udata!=null)
                    return getText(udata);
            }

            return getText(data);

        } catch (IOException e) {
            return "Sorry,read body failed!";
        }
    }

    public final static long getFsize(String path){

        if(TextUtils.isEmpty(path))
            return 0;

        long size = 0;

        try {
            size = Files.size(Paths.get(path));
        } catch (IOException e) {
            size = 0;
        }

        return size;
    }
}
