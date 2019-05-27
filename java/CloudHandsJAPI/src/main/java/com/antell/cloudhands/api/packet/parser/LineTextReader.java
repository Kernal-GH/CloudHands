package com.antell.cloudhands.api.packet.parser;


import com.antell.cloudhands.api.utils.Constants;

/**
 * Created by dell on 2017/4/6.
 */
public class LineTextReader {

    private byte[] data;
    private int pos;
    private int end;

    public LineTextReader(byte[] data,int pos,int end){

        this.data = data;
        this.pos = pos;
        this.end = end;
    }

    public int remainLen(){
        return end - pos;
    }

    public void skip(int len){

        pos += len;
        if(pos>end){
            pos = end;
        }
    }

    public boolean hasData(){

        return pos<end;
    }

    public void reset(){

        this.pos = 0;
        this.end = 0;
    }

    public boolean hasLine(){

        if(!hasData())
            return false;

        for (int i = pos; i < end; i++) {
            if (data[i] == Constants.LF) {
                return true;
            }
        }

        return false;
    }

    public String readLine() {
        // attempt to find end of line (LF)
        int start = -1;
        for (int i = pos; i < end; i++) {
            if (data[i] == Constants.LF) {
                start = i;
                break;
            }
        }
        if (start == -1) {
            /*no find a complete line*/
            return null;
        }

        int lineLen = start - pos + 1;
        int rawlineLen = lineLen;

        // discard LF if found
        lineLen--;
        // discard CR if found
        if (lineLen > 0 && data[start - 1] == Constants.CR)
            lineLen--;

        String text = new String(data,pos,lineLen);

        pos += rawlineLen;

        return text.trim();
    }

    public void updatePos(int p){

        if(p>end){
            p = end;
        }
        pos = p;
    }

    public static void main(String[] args){

        String text = " Host: www.baidu.com  \r\nServer:www.baidu.com\r\nStatus:200\nUsage:print\r\n";
        LineTextReader reader = new LineTextReader(text.getBytes(),0,text.length());

        while(reader.hasLine()){

            System.out.println(reader.readLine());
        }

    }
}

