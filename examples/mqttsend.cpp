#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "mqtt.h"

MQTT mqttClient;

using namespace std;

void onConnectFailureClient(void *context, MQTTAsync_failureData *response)
{
    mqttClient.connected = 0;
    printf("mqtt:connect failed, rc %d\n", response ? response->code : -1);
    MQTTAsync client = (MQTTAsync) context;
}

void onConnectClient(void *context, MQTTAsync_successData *response)
{
    mqttClient.connected = 1;
    //连接成功的回调，只会在第一次 connect 成功后调用，后续自动重连成功时并不会调用，因此应用需要自行保证每次 connect 成功后重新订阅
    printf("mqtt:client:connect success\n");
}

void onDisconnectClient(void *context, MQTTAsync_successData *response)
{
    mqttClient.connected = 0;
    printf("mqtt:connect disconnect\n");
}

void onPublishFailureClient(void *context, MQTTAsync_failureData *response)
{
    printf("Publish failed, rc %d\n", response ? -1 : response->code);
}

void onPublishClient(void *context, MQTTAsync_successData *response)
{
    printf("mqtt:publish:send success\n");
}

void connectionLostClient(void *context, char *cause)
{
    mqttClient.connected = 0;
    printf("mqtt:client:connection lost\n");
}

int main()
{
    char msg[1024];

    mqttClient.topic = "DEVICE/ABC";
    mqttClient.deviceId = "123";//mac地址; id不能相同
    mqttClient.onConnectFailure = onConnectFailureClient;
    mqttClient.onConnectClient = onConnectClient;
    mqttClient.onDisconnect = onDisconnectClient;
    mqttClient.onPublishFailure = onPublishFailureClient;
    mqttClient.onPublish = onPublishClient;
    mqttClient.connectionLost = connectionLostClient;

    mqttClient.init();

    while(true) {
        if(mqttClient.publish_connect()) {
            sleep(1);
            continue;
        }

        while(true) {
            sprintf(msg, "hello!! this is mqtt demo\n");

            mqttClient.publish_send(msg);

            sleep(1);

        }
        mqttClient.publish_disconnect();
    }

    return 0;
}