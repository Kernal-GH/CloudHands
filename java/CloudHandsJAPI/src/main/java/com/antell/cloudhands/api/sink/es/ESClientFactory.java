package com.antell.cloudhands.api.sink.es;



import com.antell.cloudhands.api.geoip.GeoIPConfigItem;

import java.io.IOException;

/**
 * Created by dell on 2017/4/25.
 */
public final class ESClientFactory {

    private ESClientFactory(){

    }

    public static ESClient create(ESConfigItem configItem, GeoIPConfigItem geoIPConfigItem) throws IOException {

        String type = configItem.getClientType();

        if(type.equalsIgnoreCase("bulk")){

            return new BulkESClient(configItem,geoIPConfigItem);
        }else{

            return new SimpleESClient(configItem,geoIPConfigItem);
        }
    }
}
