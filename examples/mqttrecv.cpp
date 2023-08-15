#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "mqtt.h"

MQTT mqttServer;

using namespace std;

int messageArrivedServer(void *context, char *topicName, int topicLen, MQTTAsync_message *m)
{
    printf("recv message from %s ,body is %s\n", topicName, (char *) m->payload);

    MQTTAsync_freeMessage(&m);
    MQTTAsync_free(topicName);
    return 1;
}

void onConnectFailureServer(void *context, MQTTAsync_failureData *response)
{
    if(mqttServer.connected)
        mqttServer.connected = 0;

    printf("mqtt:connect failed, rc %d\n", response ? response->code : -1);
    MQTTAsync client = (MQTTAsync) context;
}

void onSubcribeServer(void *context, MQTTAsync_successData *response)
{
    printf("subscribe success\n");
}

void onConnectServer(void *context, MQTTAsync_successData *response)
{
    if(!mqttServer.connected)
        mqttServer.connected = 1;

    //连接成功的回调，只会在第一次 connect 成功后调用，后续自动重连成功时并不会调用，因此应用需要自行保证每次 connect 成功后重新订阅
    printf("mqtt:server:connect success\n");
    MQTTAsync client = (MQTTAsync) context;
    //do sub when connect success
    MQTTAsync_responseOptions sub_opts = MQTTAsync_responseOptions_initializer;
    sub_opts.onSuccess = mqttServer.onSubcribe;
    int rc = 0;
    if ((rc = MQTTAsync_subscribe(client, mqttServer.topic.c_str(), 1, &sub_opts)) != MQTTASYNC_SUCCESS) {
        printf("Failed to subscribe, return code %d\n", rc);
    }
}

void onDisconnectServer(void *context, MQTTAsync_successData *response)
{
    if(mqttServer.connected)
        mqttServer.connected = 0;

    printf("mqtt:connect disconnect\n");
}

void onPublishFailureServer(void *context, MQTTAsync_failureData *response)
{
    printf("Publish failed, rc %d\n", response ? -1 : response->code);
}

void connectionLostServer(void *context, char *cause)
{
    if(mqttServer.connected)
        mqttServer.connected = 0;

    printf("mqtt:server:connection lost\n");
}

int main()
{
    mqttServer.topic = "DEVICE/#";
    mqttServer.deviceId = "456";//mac地址;  id不能相同
    mqttServer.messageArrived = messageArrivedServer;
    mqttServer.onConnectFailure = onConnectFailureServer;
    mqttServer.onSubcribe = onSubcribeServer;
    mqttServer.onConnectServer = onConnectServer;
    mqttServer.onDisconnect = onDisconnectServer;
    mqttServer.onPublishFailure = onPublishFailureServer;
    mqttServer.connectionLost = connectionLostServer;

    mqttServer.init();

    while(true) {
        if(mqttServer.subscript_connect()) {
            sleep(1);
           
            continue;
        }
        break;
    }
    while(true) {
        sleep(2);
    }

    
    mqttServer.subscript_disconnect();



    return 0;
}