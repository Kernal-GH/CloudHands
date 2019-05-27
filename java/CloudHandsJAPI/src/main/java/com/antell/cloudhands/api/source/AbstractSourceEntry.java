package com.antell.cloudhands.api.source;

import com.antell.cloudhands.api.utils.TextUtils;

public abstract class AbstractSourceEntry implements SourceEntry {

    protected final String obejctId;

    public AbstractSourceEntry() {
        this.obejctId = TextUtils.getUUID();
    }

    @Override
    public String getObjectId() {
        return obejctId;
    }

    public String getId(){

        return obejctId;
    }
}
