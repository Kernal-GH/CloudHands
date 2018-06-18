package com.antell.cloudhands.api.context;

import java.io.BufferedInputStream;
import java.io.FileInputStream;
import java.util.Properties;

/**
 * Created by dell on 2017/4/11.
 */
public class PropertyContext implements Context {

    private Properties configItems;

    public PropertyContext(String confPath) throws Exception {

        configItems = new Properties();
        configItems.load(new BufferedInputStream(new FileInputStream(confPath)));
    }

    @Override
    public int getInt(String key, int def) {

        String v = configItems.getProperty(key);
        return v == null ? def : Integer.parseInt(v);
    }

    @Override
    public long getLong(String key, long def) {
        String v = configItems.getProperty(key);
        return v == null ? def : Long.parseLong(v);
    }

    @Override
    public String getString(String key, String def) {
        String v = configItems.getProperty(key);
        return v == null ? def : v;
    }

    @Override
    public boolean getBoolean(String key, boolean def) {
        String v = configItems.getProperty(key);
        if (v == null) {
            return def;
        }

        if (v.equalsIgnoreCase("true")) {
            return true;
        }
        return false;
    }

}
