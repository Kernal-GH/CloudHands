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


def handle_cmd(es,cmd):

    if cmd == "del":
        del_es_index(es)


if __name__ == '__main__':

    if len(sys.argv)<3:
        print("Usage: <host> <cmd> [args]")
        sys.exit(-1)

    host = sys.argv[1]
    cmd = sys.argv[2]

    es = Elasticsearch(host)

    handle_cmd(es,cmd)


