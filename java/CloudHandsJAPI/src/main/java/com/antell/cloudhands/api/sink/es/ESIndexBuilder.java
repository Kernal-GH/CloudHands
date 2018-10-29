package com.antell.cloudhands.api.sink.es;

import com.antell.cloudhands.api.utils.TextUtils;
import org.elasticsearch.action.admin.indices.create.CreateIndexRequest;
import org.elasticsearch.action.admin.indices.exists.indices.IndicesExistsRequest;
import org.elasticsearch.action.admin.indices.exists.indices.IndicesExistsResponse;
import org.elasticsearch.action.admin.indices.mapping.put.PutMappingRequest;
import org.elasticsearch.action.index.IndexRequest;
import org.elasticsearch.client.Client;
import org.elasticsearch.client.Requests;
import org.elasticsearch.common.settings.Settings;
import org.elasticsearch.common.xcontent.XContentBuilder;

import java.io.IOException;

/**
 * Created by dell on 2018/6/25.
 */
public class ESIndexBuilder {


    private static boolean isIndexExisted(Client client, String indexName){

        IndicesExistsResponse indicesExistsResponse = client.admin().indices()
                .exists(new IndicesExistsRequest(new String[] { indexName }))
                .actionGet();
        return indicesExistsResponse.isExists();
    }

    private static void createIndex(Client client,String mappingJson,String docType, String indexName) throws IOException {

        Settings settings = Settings.builder().put("index.max_result_window", 1000000000).build();

        client.admin().indices().create(new CreateIndexRequest(indexName).settings(settings))
                .actionGet();

        PutMappingRequest putMappingRequest = Requests
                .putMappingRequest(indexName).type(docType)
                .source(mappingJson);

        client.admin().indices().putMapping(putMappingRequest).actionGet();
    }

    public static IndexRequest create(Client client,IndexNameBuilder indexNameBuilder,ESIndexEntry indexEntry) throws IOException {

        String indexPrefix = indexEntry.getIndexDB().getIndexPrefix();
        String docType = indexEntry.getIndexDB().getDocType();
        String mappingJson = indexEntry.getIndexDB().getMapping();

        String id = TextUtils.getUUID();

        XContentBuilder content = indexEntry.toJson();
        content.field("id",indexEntry.getId());

        content.endObject();

        /*build index name*/
        String indexName = indexNameBuilder.build(indexPrefix,docType);


        /*if index not existed,then create it and putting mappings*/
        if(!isIndexExisted(client,indexName)){

            createIndex(client,mappingJson,docType,indexName);
        }

        return new IndexRequest(indexName,docType,id).source(content);

    }

}
