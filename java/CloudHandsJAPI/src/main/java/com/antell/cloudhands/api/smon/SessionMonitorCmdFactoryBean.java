package com.antell.cloudhands.api.smon;

import org.springframework.beans.factory.DisposableBean;
import org.springframework.beans.factory.FactoryBean;
import org.springframework.beans.factory.InitializingBean;

/**
 * Created by dell on 2018/7/14.
 */
public class SessionMonitorCmdFactoryBean implements FactoryBean<SessionMonitorCmd>, InitializingBean, DisposableBean {

    private SessionMonitorCmd smonCmd;
    private String mmapFileName;
    private int  contextId;


    @Override
    public void destroy() throws Exception {

        if(smonCmd!=null)
            smonCmd.close();

    }

    @Override
    public SessionMonitorCmd getObject() throws Exception {

        return smonCmd;
    }

    @Override
    public Class<?> getObjectType()
    {
        return SessionMonitorCmd.class;
    }

    @Override
    public boolean isSingleton() {
        return false;
    }

    @Override
    public void afterPropertiesSet() throws Exception {

        smonCmd = new SessionMonitorCmd(contextId,mmapFileName);
        smonCmd.open();

    }

    public String getMmapFileName() {
        return mmapFileName;
    }

    public void setMmapFileName(String mmapFileName) {
        this.mmapFileName = mmapFileName;
    }

    public int getContextId() {
        return contextId;
    }

    public void setContextId(int contextId) {
        this.contextId = contextId;
    }
}
