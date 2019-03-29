host='127.0.0.1'
index="log_stream_session_tcp_*"
doc = "esdatabase_doc"
dsl = {"query":{"bool":{"must":[{"term":{"app.uprotocol":"pop3"}},{"range":{"reqDataSize":{"gte":"10"}}}],"must_not":[],"should":[]}}}

sdsl = {"query":{"bool":{"must":[{"term":{"app.uprotocol":"pop3"}},{"range":{"reqDataSize":{"gte":"10"}}}],"must_not":[],"should":[]}},"from":0,"size":1000000,"sort":[],"aggs":{}}

from elasticsearch import Elasticsearch
import base64
import sys

if len(sys.argv)>=2:
    host = sys.argv[1]

es = Elasticsearch(host)

hits = es.search(index,doc,sdsl)['hits']['hits']

userList = set()

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
        passwd = passStart[0:passEndIndex]
        userList.add(src['srcIP']+" "+src['dstIP']+" " +userName+' '+passwd)


for user in userList:
    print(user)




