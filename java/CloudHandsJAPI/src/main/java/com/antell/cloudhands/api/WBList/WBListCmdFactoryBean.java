package com.antell.cloudhands.api.WBList;

import org.springframework.beans.factory.DisposableBean;
import org.springframework.beans.factory.FactoryBean;
import org.springframework.beans.factory.InitializingBean;

public class WBListCmdFactoryBean  implements FactoryBean<WBListCmd>, InitializingBean, DisposableBean {

    private WBListCmd wbListCmd;
    private String mmapFileName;
    private int  contextId;

    @Override
    public void destroy() throws Exception {

        if(wbListCmd!=null)
            wbListCmd.close();

    }

    @Override
    public WBListCmd getObject() throws Exception {
        return wbListCmd;
    }

    @Override
    public Class<?> getObjectType() {
        return WBListCmd.class;
    }

    @Override
    public boolean isSingleton() {
        return false;
    }

    @Override
    public void afterPropertiesSet() throws Exception {

        wbListCmd = new WBListCmd(contextId,mmapFileName);
        wbListCmd.open();
    }

    public void setMmapFileName(String mmapFileName) {
        this.mmapFileName = mmapFileName;
    }

    public void setContextId(int contextId) {
        this.contextId = contextId;
    }
}
