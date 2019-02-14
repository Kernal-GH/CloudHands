package com.antell.cloudhands.api.sink.es;

import com.antell.cloudhands.api.context.Context;
import org.elasticsearch.action.index.IndexRequest;
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

    protected String[] esHosts;

    protected String clusterName;
    protected int esPort;
    protected TransportClient esClient;
    protected IndexNameBuilder indexNameBuilder;

    private IndexNameBuilder createIndexNameBuilder(String indexNameType){

        if(indexNameType.equalsIgnoreCase("time")){

            return new BasedTimeIndexNameBuilder();
        }else{
            return new SimpleIndexNameBuilder();
        }
    }

    public AbstractESClient(Context context) throws IOException {

        String esHostStr = context.getString("com.antell.cloudhands.api.esSink.esHosts","127.0.0.1");
        esHosts = esHostStr.split(",");

        esPort = context.getInt("com.antell.cloudhands.api.esSink.esPort",9300);

        clusterName =  context.getString("com.antell.cloudhands.api.esSink.clusterName","elasticsearch");

        /*create esClient*/
        Settings settings = Settings.builder()
                .put("cluster.name",clusterName).build();

        this.esClient = new PreBuiltTransportClient(settings);

        for(String host:esHosts){
            this.esClient.addTransportAddress(new InetSocketTransportAddress(InetAddress.getByName(host),esPort));
        }

        String indexNameType = context.getString("com.antell.cloudhands.api.esSink.indexNameType","time");

        indexNameBuilder = createIndexNameBuilder(indexNameType);

    }

    protected IndexRequest makeIndexRequest(ESIndexEntry indexEntry) throws IOException {

        return ESIndexBuilder.create(esClient,indexNameBuilder,indexEntry);
    }

    @Override
    public void close(){
        this.esClient.close();
    }
}
