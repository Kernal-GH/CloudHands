package com.antell.cloudhands.api.utils;

import com.google.gson.Gson;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Paths;

public class GsonUtils {


    public static final <T> T loadConfigFromJsonFile(String path,Class<T> type) throws IOException {

        Gson gson = new Gson();

        return gson.fromJson(Files.newBufferedReader(Paths.get(path)),type);
    }
}
