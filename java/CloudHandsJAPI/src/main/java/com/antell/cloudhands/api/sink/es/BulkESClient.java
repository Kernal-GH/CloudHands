package com.antell.cloudhands.api.sink.es;

import com.antell.cloudhands.api.geoip.GeoIPConfigItem;
import org.elasticsearch.action.ActionListener;
import org.elasticsearch.action.bulk.BulkProcessor;
import org.elasticsearch.action.bulk.BulkRequest;
import org.elasticsearch.action.bulk.BulkResponse;
import org.elasticsearch.action.index.IndexRequest;
import org.elasticsearch.action.update.UpdateRequest;
import org.elasticsearch.action.update.UpdateResponse;
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

    public BulkESClient(ESConfigItem configItem, GeoIPConfigItem geoIPConfigItem) throws IOException {
        super(configItem,geoIPConfigItem);
        this.bulkSize = configItem.getBulkSize();
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
    public void index(ESIndexable indexable) {

        try {

            if(indexable.getUpdateScript()==null) {
                IndexRequest indexRequest = makeIndexRequest(indexable);
                bulkProc.add(indexRequest);
            }else{
                update(indexable);
            }

        } catch (IOException e) {
            logger.error(e.getMessage());
            e.printStackTrace();
        }
    }

    private void update(ESIndexable indexable) throws IOException {

        UpdateRequest updateRequest = makeUpdateRequest(indexable);

        esClient.update(updateRequest, new ActionListener<UpdateResponse>() {
            @Override
            public void onResponse(UpdateResponse updateResponse) {

                logger.info("update es is ok--------------------------");
            }

            @Override
            public void onFailure(Exception e) {

                logger.info("update es failed --------------------------");
            }
        });
    }

    @Override
    public void close(){

        this.bulkProc.close();
        super.close();
    }
}
