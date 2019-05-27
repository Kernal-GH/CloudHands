package com.antell.cloudhands.api.sink.es;


import com.antell.cloudhands.api.geoip.GEOIPDataBase;
import com.antell.cloudhands.api.geoip.GEOIPDataBaseFactory;
import com.antell.cloudhands.api.geoip.GeoIPConfigItem;
import org.elasticsearch.action.index.IndexRequest;
import org.elasticsearch.action.update.UpdateRequest;
import org.elasticsearch.client.transport.TransportClient;
import org.elasticsearch.common.settings.Settings;
import org.elasticsearch.common.transport.InetSocketTransportAddress;
import org.elasticsearch.transport.client.PreBuiltTransportClient;

import java.io.IOException;
import java.net.InetAddress;

/**
 * Created by dell on 2017/4/21.
 */
public abstract class AbstractESClient implements ESClient {

    protected GEOIPDataBase geoipDataBase;
    protected TransportClient esClient;
    protected IndexNameBuilder indexNameBuilder;

    private IndexNameBuilder createIndexNameBuilder(String indexNameType){

        if(indexNameType.equalsIgnoreCase("time")){

            return new BasedTimeIndexNameBuilder();
        }else{
            return new SimpleIndexNameBuilder();
        }
    }

    public AbstractESClient(ESConfigItem configItem, GeoIPConfigItem geoIPConfigItem) throws IOException {

        /*create geoip database*/
        geoipDataBase = GEOIPDataBaseFactory.create(geoIPConfigItem);

        /*create esClient*/
        Settings settings = Settings.builder()
                .put("cluster.name",configItem.getClusterName()).build();

        this.esClient = new PreBuiltTransportClient(settings);

        for(String host:configItem.getEsHosts()){
            this.esClient.addTransportAddress(new InetSocketTransportAddress(InetAddress.getByName(host),configItem.getEsPort()));
        }

        indexNameBuilder = createIndexNameBuilder(configItem.getIndexTypeName());

    }

    protected IndexRequest makeIndexRequest(ESIndexable indexable) throws IOException {

        return ESIndexBuilder.create(esClient,indexNameBuilder,geoipDataBase,indexable);
    }

    protected UpdateRequest makeUpdateRequest(ESIndexable indexable) throws IOException {

        return ESIndexBuilder.createUpdateRequest(esClient,indexNameBuilder,geoipDataBase,indexable);
    }


    @Override
    public void close(){
        this.esClient.close();
    }
}
