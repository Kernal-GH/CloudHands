package com.antell.cloudhands.api.sink.kafka;

import com.antell.cloudhands.api.context.Context;

import java.util.ArrayList;
import java.util.List;

public class KafkaSink {

    private final List<KafkaPacketSourceReader> kafkaPacketSourceReaders;

    public KafkaSink(Context context) {

        kafkaPacketSourceReaders = new ArrayList<>();

        init(context);
    }

    private void init(Context context) {

        String mmaps = context.getString("com.antell.cloudhands.api.kafka.sink.source.mmapFiles", null);

        String[] mmapFiles = mmaps.split(",");

        int i = 0;

        for (String mmapFile : mmapFiles) {

            KafkaClient kafkaClient = new KafkaClient(context);
            KafkaPacketSourceReader reader = new KafkaPacketSourceReader(kafkaClient, i, mmapFile);

            kafkaPacketSourceReaders.add(reader);

            i++;
        }
    }

    public void start() {

        kafkaPacketSourceReaders.forEach(reader->reader.start());
    }

    public void stop() {

        kafkaPacketSourceReaders.forEach(reader->reader.stop());
    }

}
