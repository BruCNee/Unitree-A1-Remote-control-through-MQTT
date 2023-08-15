#include <signal.h>
#include <memory.h>
#include <stdlib.h>

// #if defined(WIN32)
// #define sleep Sleep
// #else

#include <unistd.h>
#include <openssl/hmac.h>
#include <openssl/bio.h>

//#endif

#include "mqtt.h"

MQTT::MQTT()
{
    instanceId = "";
    host = "47.115.206.127";
    groupId = "GID_DEVICE";
    deviceId = "demo";
    accessKey = "";
    secretKey = "";
    topic = "test001";
    port = 1883;
    connected = 0;
    useSSL = false;
}

void MQTT::init(void)
{
    char tempData[100];
    int len = 0;
    printf("mqtt:init:topic:%s:deviceId:%s\n", topic.c_str(), deviceId.c_str());
    //ClientID要求使用 GroupId 和 DeviceId 拼接而成，长度不得超过64个字符
    snprintf(clientIdUrl, 64, "%s@@@%s", groupId.c_str(), deviceId.c_str());
    //username和 Password 签名模式下的设置方法，参考文档 https://help.aliyun.com/document_detail/48271.html?spm=a2c4g.11186623.6.553.217831c3BSFry7
    HMAC(EVP_sha1(), (unsigned char *)secretKey.c_str(), strlen(secretKey.c_str()), (const unsigned char *)clientIdUrl, strlen(clientIdUrl), (unsigned char *)tempData, (unsigned int *)&len);
    int passWordLen = EVP_EncodeBlock((unsigned char *) passWord, (const unsigned char *)tempData, len);
    passWord[passWordLen] = '\0';
    printf("passWord is %s\n", passWord);
    snprintf(userName, 128, "Signature|%s|%s", accessKey.c_str(), instanceId.c_str());
}

int MQTT::subscript_connect(void)
{
    int cleanSession = 1;
    int rc = 0;
    //1.create client
    MQTTAsync_createOptions create_opts = MQTTAsync_createOptions_initializer;
    create_opts.sendWhileDisconnected = 0;
    create_opts.maxBufferedMessages = 10;
    char url[100];
    if (useSSL) {
        snprintf(url, 100, "ssl://%s:%d", host.c_str(), port);
    } else {
        snprintf(url, 100, "tcp://%s:%d", host.c_str(), port);
    }
    rc = MQTTAsync_createWithOptions(&client, url, clientIdUrl, MQTTCLIENT_PERSISTENCE_NONE, NULL, &create_opts);
    rc = MQTTAsync_setCallbacks(client, client, connectionLost, messageArrived, NULL);
    //2.connect to server
    MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;
    conn_opts.MQTTVersion = MQTTVERSION_3_1_1;
    conn_opts.keepAliveInterval = 60;
    conn_opts.cleansession = cleanSession;
    conn_opts.username = userName;
    conn_opts.password = passWord;
    conn_opts.onSuccess = onConnectServer;
    conn_opts.onFailure = onConnectFailure;
    conn_opts.context = client;
    //如果需要使用 SSL 加密
    if (useSSL) {
        MQTTAsync_SSLOptions ssl =MQTTAsync_SSLOptions_initializer;
        conn_opts.ssl = &ssl;
    } else {
        conn_opts.ssl = NULL;
    }
    conn_opts.automaticReconnect = 1;
    conn_opts.connectTimeout = 3;
    if ((rc = MQTTAsync_connect(client, &conn_opts)) != MQTTASYNC_SUCCESS) {
        printf("Failed to start connect, return code %d\n", rc);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int MQTT::subscript_disconnect(void)
{
    int rc = 0;
    MQTTAsync_disconnectOptions disc_opts = MQTTAsync_disconnectOptions_initializer;
    disc_opts.onSuccess = onDisconnect;
    if ((rc = MQTTAsync_disconnect(client, &disc_opts)) != MQTTASYNC_SUCCESS) {
        printf("Failed to start disconnect, return code %d\n", rc);
        return EXIT_FAILURE;
    }
    while (connected)
        sleep(1);
    MQTTAsync_destroy(&client);
    return EXIT_SUCCESS;
}

int MQTT::publish_connect(void)
{
    int cleanSession = 1;
    int rc = 0;
    //1.create client
    MQTTAsync_createOptions create_opts = MQTTAsync_createOptions_initializer;
    create_opts.sendWhileDisconnected = 0;
    create_opts.maxBufferedMessages = 10;
    char url[100];
    if (useSSL) {
        snprintf(url, 100, "ssl://%s:%d", host.c_str(), port);
    } else {
        snprintf(url, 100, "tcp://%s:%d", host.c_str(), port);
    }
    rc = MQTTAsync_createWithOptions(&client, url, clientIdUrl, MQTTCLIENT_PERSISTENCE_NONE, NULL, &create_opts);
    rc = MQTTAsync_setCallbacks(client, client, connectionLost, messageArrived, NULL);
    //2.connect to server
    MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;
    conn_opts.MQTTVersion = MQTTVERSION_3_1_1;
    conn_opts.keepAliveInterval = 60;
    conn_opts.cleansession = cleanSession;
    conn_opts.username = userName;
    conn_opts.password = passWord;
    conn_opts.onSuccess = onConnectClient;
    conn_opts.onFailure = onConnectFailure;
    conn_opts.context = client;
    //如果需要使用 SSL 加密
    if (useSSL) {
        MQTTAsync_SSLOptions ssl =MQTTAsync_SSLOptions_initializer;
        conn_opts.ssl = &ssl;
    } else {
        conn_opts.ssl = NULL;
    }
    conn_opts.automaticReconnect = 1;
    conn_opts.connectTimeout = 3;
    if ((rc = MQTTAsync_connect(client, &conn_opts)) != MQTTASYNC_SUCCESS) {
        printf("Failed to start connect, return code %d\n", rc);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int MQTT::publish_send(char *msg)
{
    int rc = 0;

    //3.publish msg
    MQTTAsync_responseOptions pub_opts = MQTTAsync_responseOptions_initializer;
    pub_opts.onSuccess = onPublish;
    pub_opts.onFailure = onPublishFailure;
    rc = MQTTAsync_send(client, topic.c_str(), strlen(msg), msg, 0, 0, &pub_opts);
    if(rc != MQTTASYNC_SUCCESS) {
        printf("mqtt:publish:failed:%s\n", msg);
        return EXIT_FAILURE;
    } else {
        printf("mqtt:publish:success:%s\n", msg);
        return EXIT_SUCCESS;
    }
}

int MQTT::publish_disconnect(void)
{
    int rc = 0;
    MQTTAsync_disconnectOptions disc_opts = MQTTAsync_disconnectOptions_initializer;
    disc_opts.onSuccess = onDisconnect;
    if ((rc = MQTTAsync_disconnect(client, &disc_opts)) != MQTTASYNC_SUCCESS) {
        printf("Failed to start disconnect, return code %d\n", rc);
        return EXIT_FAILURE;
    }
    while (connected)
        sleep(1);
    MQTTAsync_destroy(&client);
    return EXIT_SUCCESS;
}
