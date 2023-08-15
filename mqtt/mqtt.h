#ifndef MQTT_H
#define MQTT_H

#include <MQTTAsync.h>
#include <string>

using namespace std;

class MQTT {
public:
    bool useSSL;
    //实例 ID，购买后从控制台获取
    string instanceId;
    //接入点域名，从控制台获取
    string host;
    //客户端使用的 GroupID，从控制台申请
    string groupId;
    //客户端 ClientID 的后缀，由业务自行指定，只需要保证全局唯一即可
    string deviceId;
    //账号 AccessKey，从账号控制台获取
    string accessKey;
    //账号 SecretKey，从账号控制台获取
    string secretKey;
    //使用的协议端口，默认 tcp 协议使用1883，如果需要使用 SSL 加密，端口设置成8883，具体协议和端口参考文档链接https://help.aliyun.com/document_detail/44867.html?spm=a2c4g.11186623.6.547.38d81cf7XRnP0C
    int port = 1883;
    //测试收发消息的 Topic
    string topic;

    int (*messageArrived)(void *context, char *topicName, int topicLen, MQTTAsync_message *m);
    void (*onConnectFailure)(void *context, MQTTAsync_failureData *response);
    void (*onSubcribe)(void *context, MQTTAsync_successData *response);
    void (*onConnectServer)(void *context, MQTTAsync_successData *response);
    void (*onConnectClient)(void *context, MQTTAsync_successData *response);
    void (*onDisconnect)(void *context, MQTTAsync_successData *response);
    void (*onPublishFailure)(void *context, MQTTAsync_failureData *response);
    void (*onPublish)(void *context, MQTTAsync_successData *response);
    void (*connectionLost)(void *context, char *cause);

    MQTT();

    void init(void);
    int subscript_connect(void);
    int subscript_disconnect(void);
    int publish_connect(void);
    int publish_send(char *msg);
    int publish_disconnect(void);

    MQTTAsync client;
    char clientIdUrl[64];
    char passWord[100];
    char userName[128];

    int connected;
};

#endif
