package com.antell.cloudhands.api.geoip;

import com.antell.cloudhands.api.context.Context;

/**
 * Created by dell on 2018/6/26.
 */
public class GEOIPDataBaseFactory {

    private GEOIPDataBaseFactory(){


    }

    public static GEOIPDataBase create(Context context) throws GEOIPException {

        GEOIPDataBase dataBase = null;

        String dbType = context.getString("com.antell.cloudhands.api.geoip.database.type","maxmind");
        if(dbType.equalsIgnoreCase("maxmind")){

            dataBase = new MaxmindGEOIPDataBase();
        }


        if(dataBase!=null){

            dataBase.open(context);
        }

        return dataBase;
    }

}
