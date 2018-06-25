package com.antell.cloudhands.api.packet;

import com.antell.cloudhands.api.context.Context;
import com.antell.cloudhands.api.source.*;


/**
 * Created by dell on 2018/6/19.
 */
public class PacketSource extends AbstractSource {

    @Override
    public void init(Context context, Filter filter) throws SourceException {

        String mmap = context.getString("com.antell.cloudhands.api.packet.source.mmapFile",null);
        
		//Preconditions.checkArgument(mmap!=null,"must specify mmap file names to read!");

        SourceReader sourceReader = new PacketSourceReader(this,filter,mmap);

        setSourceReader(sourceReader);

    }


}
