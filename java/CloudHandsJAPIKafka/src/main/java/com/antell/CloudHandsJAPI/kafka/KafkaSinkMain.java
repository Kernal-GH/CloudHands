package com.antell.CloudHandsJAPI.kafka;

import com.antell.cloudhands.api.context.Context;
import com.antell.cloudhands.api.context.PropertyContext;
import com.antell.cloudhands.api.sink.kafka.KafkaSink;

public class KafkaSinkMain {

    public static void main(String[] args) throws Exception {

        if (args.length < 1) {

            System.err.println("Usage: KafkaSinkMain <configName>");
            System.exit(-1);
        }

        /*load config file */
        Context context = new PropertyContext(args[0]);

        /*create kafka sink*/

        final KafkaSink kafkaSink = new KafkaSink(context);

        kafkaSink.start();

        /*keepalive main thread*/
        Runtime.getRuntime().addShutdownHook(new Thread("KafkaSinkMain-shutdown-hook") {
            @Override
            public void run() {

                kafkaSink.stop();
                System.err.println("KafkaSinkMain exit -------------------------------------------------------------------------------kao");
            }
        });

        KeepAliveThread kpt = new KeepAliveThread("KafkaSinkMain");

        kpt.start();
    }

}
