package com.antell.cloudhands.api.smon;

import com.antell.cloudhands.api.statistic.StatDataReader;
import org.springframework.beans.factory.DisposableBean;
import org.springframework.beans.factory.FactoryBean;
import org.springframework.beans.factory.InitializingBean;

/**
 * Created by dell on 2018/7/14.
 */
public class SessionMonitorCmdFactoryBean implements FactoryBean<StatDataReader>, InitializingBean, DisposableBean {

    @Override
    public void destroy() throws Exception {

    }

    @Override
    public StatDataReader getObject() throws Exception {
        return null;
    }

    @Override
    public Class<?> getObjectType() {
        return null;
    }

    @Override
    public boolean isSingleton() {
        return false;
    }

    @Override
    public void afterPropertiesSet() throws Exception {

    }

}
