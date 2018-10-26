package com.antell.cloudhands.api.sink.kafka;

import com.antell.cloudhands.api.packet.PacketRecord;
import com.antell.cloudhands.api.packet.PacketRecordReader;
import com.antell.cloudhands.api.source.SourceException;
import com.antell.cloudhands.api.source.SourceReader;

import java.util.concurrent.ScheduledThreadPoolExecutor;
import java.util.concurrent.TimeUnit;

public class KafkaPacketSourceReader implements SourceReader {

    private PacketRecordReader packetRecordReader;
    private final KafkaClient kafkaClient;

    private ScheduledThreadPoolExecutor scheduledThreadPoolExecutor;
    private SourceReadRunnable sourceReadRunnable;

    public KafkaPacketSourceReader(KafkaClient kafkaClient, int id, String mmapFileName) {

        this.kafkaClient = kafkaClient;

        this.packetRecordReader = new PacketRecordReader(id, mmapFileName, null, 0, 65536);
    }

    @Override
    public int open() {

        return packetRecordReader.open();
    }

    @Override
    public PacketRecord read() {

        return packetRecordReader.read();
    }

    @Override
    public void readEnd(PacketRecord packetRecord) {

        packetRecord.reset();
    }

    @Override
    public void start() throws SourceException {

        scheduledThreadPoolExecutor = new ScheduledThreadPoolExecutor(1);

        if (open() == -1) {

            throw new SourceException("Cannot open source reader to read!");

        }
        sourceReadRunnable = new SourceReadRunnable();

        scheduledThreadPoolExecutor.scheduleAtFixedRate(sourceReadRunnable, 500, 500,
                TimeUnit.MILLISECONDS);
    }

    @Override
    public void stop() {
        close();
        scheduledThreadPoolExecutor.remove(sourceReadRunnable);
    }

    @Override
    public void close() {

        packetRecordReader.close();
    }

    private class SourceReadRunnable implements Runnable {

        @Override
        public void run() {

            while (true) {
                PacketRecord packetRecord = read();
                if (packetRecord != null) {

                    try {
                        kafkaClient.send(packetRecord.getDataWithTypeLen());
                    } catch (Exception e) {
                        e.printStackTrace();
                    } finally {
                        readEnd(packetRecord);
                    }
                } else {
                    break;
                }
            }
        }
    }
}
