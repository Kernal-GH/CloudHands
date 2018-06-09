package com.antell.cloudhands.api.statistic;

import java.io.IOException;

/**
 * Created by dell on 2018/6/8.
 */
public class TestStatDataReader {


    public static void main(String[] args) throws InterruptedException, IOException {

        if(args.length!=3){

            System.err.println("Usage:app_name <fname><buf_size>");
            System.exit(-1);
        }

        StatDataReader reader = new StatDataReader(args[1],Long.parseLong(args[2]));

        int ret = reader.open();

        if(ret!=0){
            System.err.println("Open failed!");
            System.exit(-1);
        }

        while(true){

            StatDataInput din = reader.read(0);

            if(din!=null){

                StatDataResult dataResult = new StatDataResult();
                dataResult.build(din.getDataInput());
                System.out.println(dataResult);
            }

            din.reset();

            Thread.sleep(1000);
        }
    }
}
