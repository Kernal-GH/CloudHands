package com.antell.cloudhands.api.packet.tcp.http;

import com.antell.cloudhands.api.utils.TextUtils;

import java.io.File;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.nio.file.Files;
import java.nio.file.Paths;

public class FileStorage {

    private final RandomAccessFile fileStore;

    private final static boolean isNeedMove(String path,long offset){

        return offset!=0&& !TextUtils.isEmpty(path)&& Files.exists(Paths.get(path));
    }

    public FileStorage(String path,long offset) throws IOException {

        boolean isNeedM = isNeedMove(path,offset);

        fileStore = new RandomAccessFile(new File(path),"rw");

        if(isNeedM){

            byte[] oldData = Files.readAllBytes(Paths.get(path));
            fileStore.seek(offset);
            fileStore.write(oldData);
            fileStore.seek(0);
        }


    }

    public long write(String path,long offset,boolean del) throws IOException {

        fileStore.seek(offset);
        byte[] data = Files.readAllBytes(Paths.get(path));

        fileStore.write(data);

        if(del){

            Files.delete(Paths.get(path));
        }

        fileStore.seek(0);

        return data.length;
    }

    public void close(){

        try {
            fileStore.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public long size(){

        try {
            return fileStore.length();
        } catch (IOException e) {
            return 0;
        }
    }

}
