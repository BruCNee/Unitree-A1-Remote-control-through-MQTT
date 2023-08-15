/************************************************************************
2022.7.26 NJJ finished
************************************************************************/
#include <typeinfo> 
#include "unitree_legged_sdk/unitree_legged_sdk.h"
#include <math.h>
#include <termio.h>
#include<stdio.h>
#include <iostream>
#include <unistd.h>
#include <string.h>
using namespace std;
using namespace UNITREE_LEGGED_SDK;

//mqtt

#include <stdlib.h>
#include <stdio.h>
#include "mqtt.h"

//mqtt实现
MQTT mqttServer;
MQTT Test000;

using namespace std;



class Custom
{
public:
    Custom(uint8_t level): safe(LeggedType::A1), udp(level){
        udp.InitCmdData(cmd);
    }
    void UDPRecv();
    void UDPSend();
    void RobotControl();
    void KeyboardContol();
    void MqttControl(string opts);

    Safety safe;
    UDP udp;
    HighCmd cmd = {0};
    HighState state = {0};
    int motiontime = 0;
    float dt = 0.002;     // 0.001~0.01
};


void Custom::UDPRecv()
{
    udp.Recv();
}

void Custom::UDPSend()
{  
    udp.Send();
}

void Custom::RobotControl() 
{
    motiontime += 2;
    udp.GetRecv(state);
    printf("%d   %f\n", motiontime, state.imu.quaternion[2]);

    cmd.forwardSpeed = 0.0f;
    cmd.sideSpeed = 0.0f;
    cmd.rotateSpeed = 0.0f;
    cmd.bodyHeight = 0.0f;

    cmd.mode = 0;      // 0:idle, default stand      1:forced stand     2:walk continuously
    cmd.roll  = 0;
    cmd.pitch = 0;
    cmd.yaw = 0;
  

    if(motiontime>1000 && motiontime<1500){
        cmd.mode = 1;
        cmd.roll = 0.5f;
    }

    if(motiontime>1500 && motiontime<2000){
        cmd.mode = 1;
        cmd.pitch = 0.3f;
    }

    if(motiontime>2000 && motiontime<2500){
        cmd.mode = 1;
        cmd.yaw = 0.3f;
    }

    if(motiontime>2500 && motiontime<3000){
        cmd.mode = 1;
        cmd.bodyHeight = -0.3f;
    }

    if(motiontime>3000 && motiontime<3500){
        cmd.mode = 1;
        cmd.bodyHeight = 0.3f;
    }

    if(motiontime>3500 && motiontime<4000){
        cmd.mode = 1;
        cmd.bodyHeight = 0.0f;
    }

    if(motiontime>4000 && motiontime<5000){
        cmd.mode = 2;
    }

    if(motiontime>5000 && motiontime<8500){
        cmd.mode = 2;
        cmd.forwardSpeed = 0.1f; // -1  ~ +1
    }

    if(motiontime>8500 && motiontime<12000){
        cmd.mode = 2;
        cmd.forwardSpeed = -0.2f; // -1  ~ +1
    }

    if(motiontime>12000 && motiontime<16000){
        cmd.mode = 2;
        cmd.rotateSpeed = 0.3f;   // turn
    }

    if(motiontime>16000 && motiontime<20000){
        cmd.mode = 2;
        cmd.rotateSpeed = -0.3f;   // turn
    }

    if(motiontime>20000 ){
        cmd.mode = 1;
    }
    udp.SetSend(cmd);


}

void Custom::MqttControl(string msg){
    udp.GetRecv(state);
    
    cmd.forwardSpeed = 0.0f;
    cmd.sideSpeed = 0.0f;
    cmd.rotateSpeed = 0.0f;
    cmd.bodyHeight = 0.0f;

    cmd.mode = 0;      // 0:idle, default stand      1:forced stand     2:walk continuously
    cmd.roll  = 0;
    cmd.pitch = 0;
    cmd.yaw = 0;

    string opt = "0";
    cout << "1为扭转腰部，2反方向扭转，3为修复腰部，4为前进，5为左转，6为右转，7为向后"<< endl;    
    opt = msg ;
        if (bool opt = "1"){
            cmd.mode = 1;
            cmd.roll = 0.5f;
            udp.SetSend(cmd);
            
        }
        else if (opt = "2"){
            cmd.mode = 1;
            cmd.roll = -0.5f;
            udp.SetSend(cmd);       
        }
        else if (opt = "3"){
            cmd.mode = 2;
            cmd.roll = 0.0f;
            udp.SetSend(cmd);
              
        }
        else if (opt = "4"){
            cmd.mode = 2;
            cmd.forwardSpeed=0.1f;
            udp.SetSend(cmd);
              
        }
        else if (opt = "5"){
            cmd.mode = 1;
            cmd.rotateSpeed = 0.3f;
            udp.SetSend(cmd);
              
        }
        else if (opt = "6"){
            
           cmd.mode = 2;
           cmd.rotateSpeed = -0.3f;
           udp.SetSend(cmd);
        }
        else if (opt = "7"){
            cmd.mode = 2;
            cmd.forwardSpeed=-0.1f;
            udp.SetSend(cmd);
              
        }
        else{
            cout<<"no!"<<endl;
            exit(0);

        }
        cout << "mode是"<<cmd.mode;
        cout << "向前的速递是"<<cmd.forwardSpeed;


    udp.SetSend(cmd);
    
}

void Custom::KeyboardContol(){

    udp.GetRecv(state);
 

    cmd.forwardSpeed = 0.0f;
    cmd.sideSpeed = 0.0f;
    cmd.rotateSpeed = 0.0f;
    cmd.bodyHeight = 0.0f;

    cmd.mode = 0;      // 0:idle, default stand      1:forced stand     2:walk continuously
    cmd.roll  = 0;
    cmd.pitch = 0;
    cmd.yaw = 0;




    int opt = 0;
		
    cout << "1为扭转腰部，2反方向扭转，3为修复腰部，4为前进，5为左转，6为右转，7为向后"<< endl;    
    cin >> opt;
        if (opt == 1){
            cmd.mode = 1;
            cmd.roll = 0.5f;
            
        }
        else if (opt == 2){
            cmd.mode = 1;
            cmd.roll = -0.5f;       
        }
        else if (opt == 3){
            cmd.mode = 2;
            cmd.roll = 0.0f;
              
        }
        else if (opt == 4){
            cmd.mode = 2;
            cmd.forwardSpeed=0.1f;
              
        }
        else if (opt == 5){
            cmd.mode = 1;
            cmd.rotateSpeed = 0.3f;
              
        }
        else if (opt == 6){
            
           cmd.mode = 2;
           cmd.rotateSpeed = -0.3f;
        }
        else if (opt == 7){
            cmd.mode = 2;
            cmd.forwardSpeed=-0.1f;
              
        }
        else{
            cout<<"no!"<<endl;
            exit(0);

        }
        cout << "mode是"<<cmd.mode;
        cout << "向前的速递是"<<cmd.forwardSpeed;
        udp.SetSend(cmd);

}



// char GetMessage(void *opt){
//      char Mes2 = (char*)->opt ;
//      return Mes2;
// }

//确定狗的状态后接受消息
int messageArrivedServer(void *context, char *topicName, int topicLen, MQTTAsync_message *m)
{
    printf("recv message from %s ,body is %s\n", topicName, (char *) m->payload);
    cout<<"信息三"<<(char *) m->payload;
    string msg =(char *) m->payload;
    
    //char message = *(char*)m->payload;
    
    cout<<"解析为"<<msg<<endl;
    
        Custom custom2(HIGHLEVEL);
        // LoopFunc loop_MqttControl("MqttControl", custom2.dt,    boost::bind(&Custom::MqttControl, &custom2));
        // LoopFunc loop_udpSend("udp_send",     custom2.dt, 3, boost::bind(&Custom::UDPSend,      &custom2));
        // LoopFunc loop_udpRecv("udp_recv",     custom2.dt, 3, boost::bind(&Custom::UDPRecv,      &custom2));
        // loop_udpSend.start();
        // loop_udpRecv.start();
        // loop_MqttControl.start();
        // while(1){
        // sleep(10);
        // };
        custom2.MqttControl(msg);

        
    
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
int main(void) 
{
    std::cout << "Communication level is set to HIGH-level." << std::endl
              << "WARNING: Make sure the robot is standing on the ground." << std::endl
              << "Press Enter to continue..." << std::endl
              <<"!!!NJJ has changed the code!!!"<< std::endl;
              
    std::cin.ignore();

    Custom custom(HIGHLEVEL);
    
    cout <<"输入1进入调试模式"<< endl;
    cout <<"输入2进入键盘模式"<< endl;
    cout <<"输入3进入mqtt模式"<< endl;

    int choice = 0;
    cin >> choice;


    if (choice == 1){
        
        LoopFunc loop_control("control_loop", custom.dt,    boost::bind(&Custom::RobotControl, &custom));
        LoopFunc loop_udpSend("udp_send",     custom.dt, 3, boost::bind(&Custom::UDPSend,      &custom));
        LoopFunc loop_udpRecv("udp_recv",     custom.dt, 3, boost::bind(&Custom::UDPRecv,      &custom));
        loop_udpSend.start();
        loop_udpRecv.start();
        loop_control.start();


        while(1){
        sleep(10);
        };
    
    }


    else if(choice == 2){
        LoopFunc loop_keyboard_control("keyboard_control", custom.dt,    boost::bind(&Custom::KeyboardContol, &custom));
        LoopFunc loop_udpSend("udp_send",     custom.dt, 3, boost::bind(&Custom::UDPSend,      &custom));
        LoopFunc loop_udpRecv("udp_recv",     custom.dt, 3, boost::bind(&Custom::UDPRecv,      &custom));
        loop_udpSend.start();
        loop_udpRecv.start();
        loop_keyboard_control.start();
        while(1){
        sleep(10);
        };
    }
    else if(choice == 3){
        mqttServer.topic = "/test001";
    mqttServer.deviceId = "456";//mac地址;  id不能相同
    mqttServer.messageArrived = messageArrivedServer;
    mqttServer.onConnectFailure = onConnectFailureServer;
    mqttServer.onSubcribe = onSubcribeServer;
    mqttServer.onConnectServer = onConnectServer;
    mqttServer.onDisconnect = onDisconnectServer;
    mqttServer.onPublishFailure = onPublishFailureServer;
    mqttServer.connectionLost = connectionLostServer;
    Test000.messageArrived = messageArrivedServer;
    mqttServer.init();
    while(true){
    mqttServer.subscript_connect();
    // printf("信息是%s\n",(char*)message);
    
    sleep(10);
         
    }
    


    // while(true) {
    
    //     if(mqttServer.subscript_connect()) {
    
    //         sleep(1);

    //          continue;
    //     }
    //     break;
    // }
    // while(true) {
    //     sleep(2);
    // }
    
    mqttServer.subscript_disconnect();
    
   
    }
    else{
        cout<<"Stop"<<endl;
        
    }
    

    return 0; 
}
