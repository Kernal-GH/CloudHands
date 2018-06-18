package com.antell.cloudhands.api.context;

/**
 * Created by dell on 2017/4/11.
 */
public interface Context {


    /**
     * Get a Int value from config context by specified key,
     * if not existed,than return default value
     * @param k the config item key
     * @param d the default value
     * @return if config item existed by @k,the return it,otherwise
     * return default @d
     */
    int getInt(String k, int d);

    /**
     * Get a Long value from config context by specified key,
     * if not existed,than return default value
     * @param k the config item key
     * @param d the default value
     * @return if config item existed by @k,the return it,otherwise
     * return default @d
     */
    long getLong(String k, long d);

    /**
     * Get a String value from config context by specified key,
     * if not existed,than return default value
     * @param k the config item key
     * @param d the default value
     * @return if config item existed by @k,the return it,otherwise
     * return default @d
     */
    String getString(String k, String d);

    /**
     * Get a boolean value from config context by specified key,
     * if not existed,than return default value
     * @param k the config item key
     * @param d the default value
     * @return if config item existed by @k,the return it,otherwise
     * return default @d
     */
    boolean getBoolean(String k, boolean d);
}
