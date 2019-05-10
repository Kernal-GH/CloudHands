#!/usr/bin/python
host = "127.0.0.1"

from elasticsearch import Elasticsearch
import sys
import base64
import time
import datetime

def del_by_query(es):
    if len(sys.argv)<6:
        print("Usage:<host><cmd><index><docType><field><value>")
        sys.exit(-1)
    index = sys.argv[3]
    doc = sys.argv[4]
    field = sys.argv[5]
    value = sys.argv[6]
    query = {
            "query": {
                    "term" : { field: value  }

                }

            }
    print(query)
    print(es.count(index,doc,query)['count'])
    es.delete_by_query(index,query,doc)

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

def get_http_unusual_server_and_port(es):

    index = "log_stream_session_tcp_*"
    doc = "esdatabase_doc"
    query = {
            "query":{
                "bool":{
                    "must_not":[
                            {"term":{"dstPort":80}},
                            {"term":{"dstPort":8080}}
                        ],
                    "must":{
                            "term":{"app.uprotocol":"http"}
                        }
                }
            }
        }

    count = es.count(index,doc,query)['count']

    if count == 0:
        print("No HTTP Servers Info that port is not 80 and 8080!")
    else:
        aggs = {
            "group_by_dstIP":{
                "terms":{
                    "field":"dstIP",
                    "order":{"_count":"desc"},
                    "size":count
                    },

                    "aggs":{
                        "group_by_dstPort":{
                            "terms":{
                                    "field":"dstPort",
                                    "order":{"_count":"desc"},
                                    "size":count
                                }
                            }
                        }
                }
            }
        query['size']=0
        query['aggs']=aggs


        results = es.search(index,doc,query)['aggregations']['group_by_dstIP']['buckets']

        for ipkey in results:
            ip = ipkey['key']
            ports = "[ "
            for port in ipkey['group_by_dstPort']['buckets']:
                ports= ports+str(port['key'])+" "
            ports= ports+"]"

            print(ip+" "+ports)

def get_mail_user_passwd(es):
    #from tcp stream session get pop3 username and passwd
    stream_index = "log_stream_session_tcp_*"
    mail_index = "log_tcp_session_mail_*"
    doc = "esdatabase_doc"
    userList = set()
    stream_query = {
            "query":{
                "bool":{
                    "must":[
                        {"term":{"app.uprotocol":"pop3"}},
                        {"range":{"reqDataSize":{"gte":10}}}
                        ]
                    }
                }
            }

    count = es.count(stream_index,doc,stream_query)['count']

    if count>0:
        stream_query['from']=0
        stream_query['size']=count

        hits = es.search(stream_index,doc,stream_query)['hits']['hits']

        for hit in hits:
            src = hit['_source']
            reqData = src['reqData']
            text = base64.b64decode(reqData)

            userIndex = text.find('USER')
            passIndex = text.find('PASS')
            if userIndex != -1 and passIndex != -1:
                userName = text[userIndex+5:passIndex-2]
                passStart = text[passIndex+5:]
                passEndIndex = passStart.find('\n')
                passEndIndexR = passStart.find('\r')
                pssInd = passEndIndex
                if passEndIndexR!=-1:
                    pssInd = passEndIndexR

                passwd = passStart[0:pssInd]
                userList.add(userName+' '+passwd)

    #from mail session
    mail_query = {
            "query":{
                    "match_all":{}
                }
            }
    count = es.count(mail_index,doc,mail_query)['count']
    if count>0:
        mail_query['size'] = 0
        mail_query['aggs'] = {
                "group_by_user":{
                    "terms":{
                            "field":"userName",
                            "size":count
                        },
                    "aggs":{
                        "group_by_passwd":{
                            "terms":{
                                    "field":"passwd",
                                    'size':count
                                }
                            }
                        }
                    }
                }
        mail_results = es.search(mail_index,doc,mail_query)['aggregations']['group_by_user']['buckets']
        for userNameKey in mail_results:
            user = userNameKey['key']
            for passwd in userNameKey['group_by_passwd']['buckets']:
                pas = passwd['key']

                if len(pas)>=2 and pas[0]=='"' and pas[len(pas)-1]=='"':
                    pas=pas[1:len(pas)-1]

                userList.add(user+' '+pas)


    for user in userList:
        print(user)

def count(es):
    index = None

    if len(sys.argv)>=4:
        index = sys.argv[3]

    print(es.count(index)['count'])


def catIndices(es):
    index = "*"
    if len(sys.argv)>=4:
        index = sys.argv[3]

    results = es.cat.indices(index,v=True)
    print(results)

def getIndices(es):
    index = "*"
    if len(sys.argv)>=4:
        index = sys.argv[3]

    return es.indices.get(index).keys()

def getClosedIndices(es):
    index = "*"
    if len(sys.argv)>=4:
        index = sys.argv[3]
    results = es.cat.indices(index).split("\n")

    mres = []
    for k in results:
        if len(k)==0 or k.find('close')==-1:
            continue

        i = 0
        ksplits = list(filter(lambda s:False if len(s) == 0 else True, k.split(' ')))
        for ki in ksplits:
            if ki == 'close':
                mres.append(ksplits[i+1])
                break
            i=i+1

    return mres

def getOpenedIndices(es):
    index = "*"
    if len(sys.argv)>=4:
        index = sys.argv[3]
    results = es.cat.indices(index).split("\n")

    mres = []
    for k in results:
        if len(k)==0 or k.find('open')==-1:
            continue

        i = 0
        ksplits = list(filter(lambda s:False if len(s) == 0 else True, k.split(' ')))
        for ki in ksplits:
            if ki == 'open':
                mres.append(ksplits[i+1])
                break
            i=i+1

    return mres

def openIndices(es):
    index = "*"
    if len(sys.argv)>=4:
        index = sys.argv[3]

    indices = getClosedIndices(es)
    for k in indices:
        es.indices.open(k,timeout='30m')

def closeIndices(es):
    index = "*"
    if len(sys.argv)>=4:
        index = sys.argv[3]

    indices = getOpenedIndices(es)
    for k in indices:
        es.indices.close(k,timeout='30m')


def getTimeFromIndex(index):
    dstr = index[index.rfind('_')+1:]
    ta = time.strptime(dstr, "%Y.%m.%d")
    return int(time.mktime(ta))

def getTimeFromDateNumber(dn):
    dayAgo = (datetime.datetime.now() - datetime.timedelta(days = dn))
    return int(time.mktime(dayAgo.timetuple()))

def isOldIndex(index,oldTime):
    indexTime = getTimeFromIndex(index)
    return indexTime<=oldTime

def closeIndicesWithDate(es):
    if len(sys.argv)<5:
        print('Usage: <host> <cmd> <index> <dateNumber>')
        sys.exit(-1)
    index = sys.argv[3]
    dn = int(sys.argv[4])

    oldTime = getTimeFromDateNumber(dn)

    indices = getOpenedIndices(es)
    for ind in indices:
        if isOldIndex(ind,oldTime):
            es.indices.close(ind,timeout='30m')

def handle_cmd(es,cmd):

    if cmd == "del":
        del_es_index(es)
    elif cmd == "term_topN":
        find_terms_topN(es)
    elif cmd == "http_servers":
        get_http_unusual_server_and_port(es)
    elif cmd == 'mail_users':
        get_mail_user_passwd(es)
    elif cmd == 'count':
        count(es)
    elif cmd == 'del_by_query':
        del_by_query(es)
    elif cmd == 'cat':
        catIndices(es)
    elif cmd == 'open':
        openIndices(es)
    elif cmd == 'close':
        closeIndices(es)
    elif cmd == 'closeWithDate':
        closeIndicesWithDate(es)

if __name__ == '__main__':

    if len(sys.argv)<3:
        print("Usage: <host> <cmd> [args]")
        sys.exit(-1)

    host = sys.argv[1]
    cmd = sys.argv[2]

    es = Elasticsearch(host)

    handle_cmd(es,cmd)


