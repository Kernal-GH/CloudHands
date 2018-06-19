package com.antell.cloudhands.api.packet;

import com.antell.cloudhands.api.context.Context;
import com.antell.cloudhands.api.source.*;
import com.google.common.base.Preconditions;


/**
 * Created by dell on 2018/6/19.
 */
public class PacketSource extends AbstractSource {

    @Override
    public void init(Context context, Filter filter) throws SourceException {

        String mmap = context.getString("com.antell.cloudhands.api.packet.source.mmapFiles",null);
        Preconditions.checkArgument(mmap!=null,"must specify mmap file names to read!");

        String[] mmapFiles = mmap.split(",");

        for(String mmapFileName :mmapFiles){

            SourceReader sourceReader = new PacketSourceReader(this,filter,mmapFileName);
            sourceReaders.add(sourceReader);
        }
    }
}
