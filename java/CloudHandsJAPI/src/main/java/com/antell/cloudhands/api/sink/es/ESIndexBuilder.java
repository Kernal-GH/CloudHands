package com.antell.cloudhands.api.sink.es;

import com.antell.cloudhands.api.geoip.GEOIPDataBase;
import com.antell.cloudhands.api.geoip.GEOIPItem;
import com.antell.cloudhands.api.utils.TextUtils;
import org.elasticsearch.action.admin.indices.create.CreateIndexRequest;
import org.elasticsearch.action.admin.indices.exists.indices.IndicesExistsRequest;
import org.elasticsearch.action.admin.indices.exists.indices.IndicesExistsResponse;
import org.elasticsearch.action.admin.indices.mapping.put.PutMappingRequest;
import org.elasticsearch.action.index.IndexRequest;
import org.elasticsearch.action.update.UpdateRequest;
import org.elasticsearch.client.Client;
import org.elasticsearch.client.Requests;
import org.elasticsearch.common.settings.Settings;
import org.elasticsearch.common.xcontent.XContentBuilder;
import org.elasticsearch.common.xcontent.XContentFactory;

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

    private final static void appendGeoip(GEOIPDataBase dataBase, XContentBuilder pcb, String objectName, String ip) throws IOException {

        if(TextUtils.isEmpty(ip))
            return;

        GEOIPItem item = dataBase.query(ip);
        if(item!=null){

            item.appendToJson(pcb,objectName);
        }

    }

    public static IndexRequest create(Client client, IndexNameBuilder indexNameBuilder, GEOIPDataBase dataBase,ESIndexable indexable) throws IOException {

        String indexPrefix = indexable.getIndexNamePrefix();
        String docType = indexable.getIndexDocType();
        String mappingJson = indexable.getIndexMapping();

        String id = indexable.getId();

        XContentBuilder cb = XContentFactory.jsonBuilder();
        cb.startObject();

        cb.field("id",id);
        indexable.dataToJson(cb);
        appendGeoip(dataBase,cb,"srcIPLocation",indexable.getSrcIP());
        appendGeoip(dataBase,cb,"dstIPLocation",indexable.getDstIP());

        cb.endObject();

        /*build index name*/

        String indexName;
        if(indexable.indexISConstants())
            indexName = indexPrefix;
        else
            indexName = indexNameBuilder.build(indexPrefix,docType);


        /*if index not existed,then create it and putting mappings*/
        if(!isIndexExisted(client,indexName)){

            createIndex(client,mappingJson,docType,indexName);
        }

        return new IndexRequest(indexName,docType,id).source(cb);

    }

    public static UpdateRequest createUpdateRequest(Client client, IndexNameBuilder indexNameBuilder, GEOIPDataBase dataBase,ESIndexable indexable) throws IOException {

        IndexRequest indexRequest = create(client,indexNameBuilder,dataBase,indexable);

        UpdateRequest updateRequest = new UpdateRequest(indexRequest.index(), indexRequest.type(),indexable.getId());

        updateRequest.script(indexable.getUpdateScript());

        updateRequest.upsert(indexRequest);

        return updateRequest;
    }

}
