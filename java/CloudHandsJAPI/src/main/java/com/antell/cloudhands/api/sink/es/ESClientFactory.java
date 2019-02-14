package com.antell.cloudhands.api.sink.es;

import com.antell.cloudhands.api.context.Context;

import java.io.IOException;

/**
 * Created by dell on 2017/4/25.
 */
public final class ESClientFactory {

    private ESClientFactory(){

    }

    public static ESClient create(Context context, String type) throws IOException {

        if(type.equalsIgnoreCase("bulk")){

            return new BulkESClient(context);
        }else{

            return new SimpleESClient(context);
        }
    }
}
