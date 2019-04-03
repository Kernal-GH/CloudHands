host = "127.0.0.1"

from elasticsearch import Elasticsearch
import sys

def del_es_index(es):

    if len(sys.argv)<4:
        print("Usage: <host> <cmd> <index>")
        sys.exit(-1)

    index = sys.argv[3]
    indices = es.indices.get(index).keys()

    for ind in indices:
        es.indices.delete(ind)

def find_terms_topN(es):

    if len(sys.argv)<8:
        print("Usage:<host><cmd><index><doctype><field><topN><isDesc>")
        sys.exit(-1)

    index = sys.argv[3]
    doc = sys.argv[4]
    field = sys.argv[5]

    topN = int(sys.argv[6])

    orderS ="asc"
    if sys.argv[7] == "true":
        orderS = "desc"

    query = {
            "query": {
                    "match_all": {}

                },
              "size":0,
              "aggs": {
                  "group_terms": {
                      "terms": {"field": field,
                       "order" : { "_count" : orderS},
                        "size":topN
                        }
                      }
                  }
              }

    results = es.search(index,doc,query)['aggregations']['group_terms']['buckets']

    for item in results:
        print(item['key'])


def handle_cmd(es,cmd):

    if cmd == "del":
        del_es_index(es)
    elif cmd == "term_topN":
        find_terms_topN(es)


if __name__ == '__main__':

    if len(sys.argv)<3:
        print("Usage: <host> <cmd> [args]")
        sys.exit(-1)

    host = sys.argv[1]
    cmd = sys.argv[2]

    es = Elasticsearch(host)

    handle_cmd(es,cmd)


