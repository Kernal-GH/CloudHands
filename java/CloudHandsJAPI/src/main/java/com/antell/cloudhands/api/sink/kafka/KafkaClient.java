package com.antell.cloudhands.api.sink.kafka;

import com.antell.cloudhands.api.context.Context;
import com.google.common.base.Preconditions;
import org.apache.kafka.clients.producer.Callback;
import org.apache.kafka.clients.producer.KafkaProducer;
import org.apache.kafka.clients.producer.ProducerRecord;
import org.apache.kafka.clients.producer.RecordMetadata;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * Created by shajf on 15-11-22.
 */
public class KafkaClient {
    private static final Logger logger = LoggerFactory.getLogger(KafkaClient.class);

    private KafkaProducer<String,byte[]> producer;
    private String pnode;
    private boolean isAsync;
    private String topic;

    public KafkaClient(Context context){

        topic = context.getString("com.antell.cloudhands.api.kafka.sink.client.topic", null);
        Preconditions.checkState(topic != null, "Configuration must specify a kafka topic");

        pnode = context.getString("com.antell.cloudhands.api.kafka.sink.client.pnode", "127.0.0.1");
        isAsync = context.getBoolean("com.antell.cloudhands.api.kafka.sink.client.isAsync",true);


        producer = KafkaUtil.getKafkaProducer(context);

    }

    public void send(byte[] content) throws Exception {

        long startTime = System.currentTimeMillis();

        if(isAsync){
            producer.send(new ProducerRecord<String,byte[]>(topic,pnode,content),
                    new KafkaClientCallBack(startTime));
        }else{

            RecordMetadata recordMetadata = producer.send(new ProducerRecord<String, byte[]>(topic, pnode, content)).get();

            long endTime = System.currentTimeMillis();
            logger.debug("send a message par["+recordMetadata.partition()+"]"+",spend time["+(endTime-startTime)+"]");

        }
    }

    public void flush() {

    }

    public void close() {

    }

    private class KafkaClientCallBack implements Callback {

        private long startTime;

        public KafkaClientCallBack(long startTime){
            this.startTime = startTime;
        }

        public void onCompletion(RecordMetadata recordMetadata, Exception e) {

            long endTime = System.currentTimeMillis();

            logger.debug("send a message par["+recordMetadata.partition()+"]"+",spend time["+(endTime-startTime)+"]");

        }
    };
}
