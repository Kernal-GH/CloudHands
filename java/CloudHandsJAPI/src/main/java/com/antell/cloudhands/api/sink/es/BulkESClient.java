package com.antell.cloudhands.api.sink.es;

import com.antell.cloudhands.api.context.Context;
import org.elasticsearch.action.bulk.BulkProcessor;
import org.elasticsearch.action.bulk.BulkRequest;
import org.elasticsearch.action.bulk.BulkResponse;
import org.elasticsearch.action.index.IndexRequest;
import org.elasticsearch.common.unit.TimeValue;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.io.IOException;

/**
 * Created by dell on 2017/4/25.
 */
public class BulkESClient extends AbstractESClient{

    private Logger logger = LoggerFactory.getLogger(BulkESClient.class);

    private BulkProcessor bulkProc;
    private int bulkSize;

    public BulkESClient(Context context) throws IOException {
        super(context);
        this.bulkSize = context.getInt("com.antell.cloudhands.api.esSink.bulkSize",1000);
        createBulkProc();
    }

    private void createBulkProc(){

        bulkProc = BulkProcessor.builder(esClient, new BulkProcessor.Listener() {
            @Override
            public void beforeBulk(long l, BulkRequest bulkRequest) {
                logger.info("before send data to es---------------------------------");
            }

            @Override
            public void afterBulk(long l, BulkRequest bulkRequest, BulkResponse bulkResponse) {

                logger.info(("Sent data to es :" + (bulkResponse.hasFailures() ? "failed" : "ok")));
            }

            @Override
            public void afterBulk(long l, BulkRequest bulkRequest, Throwable throwable) {

            }
        }).setBulkActions(bulkSize)
                .setFlushInterval(TimeValue.timeValueSeconds(5))
                .setConcurrentRequests(1)
                .build();

    }

    @Override
    public void index(ESIndexEntry indexEntry) {

        try {
            IndexRequest indexRequest =  makeIndexRequest(indexEntry);

            bulkProc.add(indexRequest);
        } catch (IOException e) {
            logger.error(e.getMessage());
            e.printStackTrace();
        }
    }

    @Override
    public void close(){

        this.bulkProc.close();
        super.close();
    }
}
