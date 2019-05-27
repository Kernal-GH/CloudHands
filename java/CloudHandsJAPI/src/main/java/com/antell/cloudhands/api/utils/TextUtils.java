package com.antell.cloudhands.api.utils;

import java.io.DataInput;
import java.io.IOException;
import java.util.Base64;
import java.util.Iterator;
import java.util.List;
import java.util.UUID;

public final class TextUtils {

    public static void addInt(StringBuffer sb,String k,int v){

        sb.append(k);
        sb.append(":");
        sb.append(v);
        sb.append("\n");

    }

    public static void addLong(StringBuffer sb,String k,long v){

        sb.append(k);
        sb.append(":");
        sb.append(v);
        sb.append("\n");
    }

    public static void addDouble(StringBuffer sb,String k,double v){

        sb.append(k);
        sb.append(":");
        sb.append(v);
        sb.append("\n");
    }

    public static void addText(StringBuffer sb,String k,String v){

        sb.append(k);
        sb.append(":");
        sb.append(v);
        sb.append("\n");
    }

    public static void addList(StringBuffer sb, String k, List list){

        Iterator iter = list.iterator();

        sb.append(k);
        sb.append(":[");
        while(iter.hasNext()){

            sb.append(iter.next().toString());
            if(iter.hasNext())
                sb.append(",");
        }
        sb.append("]\n\n");
    }

    public static String readString(DataInput input) throws IOException {

        long len = input.readLong();
        if(len == 0)
            return "";

        StringBuffer sb = new StringBuffer();

        for(int i = 0;i<len;i++){
            byte c = input.readByte();
            sb.append(c);
        }

        return sb.toString();
    }

    /**
     * Returns true if the parameter is null or of zero length
     */
    public static boolean isEmpty(final CharSequence s) {
        if (s == null) {
            return true;
        }
        return s.length() == 0;
    }

    /**
     * Returns true if the parameter is null or contains only whitespace
     */
    public static boolean isBlank(final CharSequence s) {
        if (s == null) {
            return true;
        }
        for (int i = 0; i < s.length(); i++) {
            if (!Character.isWhitespace(s.charAt(i))) {
                return false;
            }
        }
        return true;
    }


    public static boolean containsBlanks(final CharSequence s) {
        if (s == null) {
            return false;
        }
        for (int i = 0; i < s.length(); i++) {
            if (Character.isWhitespace(s.charAt(i))) {
                return true;
            }
        }
        return false;
    }

    public static String getStrValue(String v){

        return v==null?"":v;
    }

    public static String getUUID(){

        return UUID.randomUUID().toString().replace("-","");
    }

    public static final int getLineIndex(String line,int offset){

        int rIndex = line.indexOf('\r',offset);
        int nIndex = line.indexOf('\n',offset);

        return rIndex!=-1?rIndex:nIndex;
    }

    public static final String decodeBase64(String text){

        try {

            return new String(Base64.getDecoder().decode(text));
        }catch (Exception e){

            return null;
        }
    }

}
