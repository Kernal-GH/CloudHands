package com.antell.cloudhands.api.statistic;

import org.springframework.beans.factory.DisposableBean;
import org.springframework.beans.factory.FactoryBean;
import org.springframework.beans.factory.InitializingBean;

/**
 * Created by dell on 2018/6/8.
 */
public class StatDataReaderFactoryBean implements FactoryBean<StatDataReader>, InitializingBean, DisposableBean {

    private StatDataReader statDataReader;
    private String mmapFileName;
    private int bsize;

    @Override
    public void destroy() throws Exception {

        if(statDataReader!=null){

            statDataReader.close();
        }

    }

    @Override
    public StatDataReader getObject() throws Exception {
        return statDataReader;
    }

    @Override
    public Class<?> getObjectType() {
        return StatDataReader.class;
    }

    @Override
    public boolean isSingleton() {
        return true;
    }

    @Override
    public void afterPropertiesSet() throws Exception {

        statDataReader = new StatDataReader(mmapFileName,bsize);

        if(-1 == statDataReader.open()){
            throw new Exception("Cannot open mmapFileName:"+mmapFileName);
        }
    }

    public void setMmapFileName(String mmapFileName) {
        this.mmapFileName = mmapFileName;
    }

    public void setBsize(int bsize) {
        this.bsize = bsize;
    }
}
