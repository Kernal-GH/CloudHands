package com.antell.cloudhands.api.sink.kafka;

import com.antell.cloudhands.api.context.Context;
import kafka.consumer.Consumer;
import kafka.javaapi.consumer.ConsumerConnector;
import org.apache.kafka.clients.consumer.ConsumerConfig;
import org.apache.kafka.clients.producer.KafkaProducer;
import org.apache.kafka.clients.producer.ProducerConfig;

import java.util.Properties;

/**
 * Created by shajf on 15-12-7.
 */
public class KafkaUtil {

    private KafkaUtil(){}

    public final static KafkaProducer<String,byte[]> getKafkaProducer(Context context){

        String brokers = context.getString("com.antell.cloudhands.api.kafka.sink.client.brokers","127.0.0.1:9092");

        Properties props = new Properties();

        props.put(ProducerConfig.BOOTSTRAP_SERVERS_CONFIG,brokers);
        props.put(ProducerConfig.KEY_SERIALIZER_CLASS_CONFIG,"org.apache.kafka.common.serialization.StringSerializer");
        props.put(ProducerConfig.VALUE_SERIALIZER_CLASS_CONFIG,"org.apache.kafka.common.serialization.ByteArraySerializer");

        return new KafkaProducer<String, byte[]>(props);
    }

    public final static ConsumerConnector getKafkaConsumer(Context context){

        Properties prop = new Properties();
        String bks = context.getString("com.antell.cloudhands.api.kafka.source.brokers","127.0.0.1:9092");
        String zks = context.getString("com.antell.cloudhands.api.kafka.source.zks","127.0.0.1:2181");
        String gid = context.getString("com.antell.cloudhands.api.kafka.source.groupID", "kafka_consumer");
        String cid = context.getString("com.antell.cloudhands.api.kafka.source.clientID",null);

        prop.put("zookeeper.connect",zks);
        prop.put(ConsumerConfig.BOOTSTRAP_SERVERS_CONFIG,bks);
        prop.put(ConsumerConfig.GROUP_ID_CONFIG,gid);
        prop.put(ConsumerConfig.CLIENT_ID_CONFIG,cid);
        prop.put(ConsumerConfig.ENABLE_AUTO_COMMIT_CONFIG,"true");
        prop.put(ConsumerConfig.AUTO_COMMIT_INTERVAL_MS_CONFIG,"10000");
        prop.put(ConsumerConfig.SESSION_TIMEOUT_MS_CONFIG,"30000");

        //prop.put(ConsumerConfig.KEY_DESERIALIZER_CLASS_CONFIG,
          //      "org.apache.kafka.common.serialization.StringDeserializer");

        //prop.put(ConsumerConfig.VALUE_DESERIALIZER_CLASS_CONFIG,
          //      "org.apache.kafka.common.serialization.ByteArrayDeserializer");

        /*create consumer connector*/
        return Consumer.createJavaConsumerConnector(new kafka.consumer.ConsumerConfig(prop));

    }

}
