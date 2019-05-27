package com.antell.cloudhands.api.geoip;


/**
 * Created by dell on 2018/6/26.
 */
public class GEOIPDataBaseFactory {

    private GEOIPDataBaseFactory(){


    }

    public static GEOIPDataBase create(GeoIPConfigItem configItem) throws GEOIPException {

        GEOIPDataBase dataBase = null;

        String dbType = configItem.getDbType();

        if(dbType.equalsIgnoreCase("maxmind")){

            dataBase = new MaxmindGEOIPDataBase();
        }

        if(dataBase!=null){

            dataBase.open(configItem.getDbPath());
        }

        return dataBase;
    }

}
