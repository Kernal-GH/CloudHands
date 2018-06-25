package com.antell.cloudhands.api.sink.es;

import com.antell.cloudhands.api.context.Context;
import org.elasticsearch.action.index.IndexRequest;
import org.elasticsearch.action.index.IndexResponse;

import java.io.IOException;

/**
 * Created by dell on 2017/4/25.
 */
public class SimpleESClient extends AbstractESClient{


    public SimpleESClient(Context context) throws IOException {
        super(context);
    }

    @Override
    public void index(ESIndexEntry indexEntry) {

        IndexRequest indexRequest = null;
        try {
            indexRequest = makeIndexRequest(indexEntry);
            IndexResponse indexResponse = esClient.index(indexRequest).actionGet();

        } catch (IOException e) {
            e.printStackTrace();
        }


    }

}
