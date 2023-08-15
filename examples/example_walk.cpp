/************************************************************************
Copyright (c) 2020, Unitree Robotics.Co.Ltd. All rights reserved.
Use of this source code is governed by the MPL-2.0 license, see LICENSE.
************************************************************************/

#include "unitree_legged_sdk/unitree_legged_sdk.h"
#include <math.h>
#include <termio.h>
#include<stdio.h>
#include <iostream>
#include <unistd.h>
#include <string.h>
//#define getchar() getc(stdin)
using namespace std;
using namespace UNITREE_LEGGED_SDK;

/*int getch()
{
	int ch;
	struct termios oldt;
	struct termios newt;

	// Store old settings, and copy to new settings
	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;

	// Make required changes and apply the settings
	newt.c_lflag &= ~(ICANON | ECHO);
	newt.c_iflag |= IGNBRK;
	newt.c_iflag &= ~(INLCR | ICRNL | IXON | IXOFF);
	newt.c_lflag &= ~(ICANON | ECHO | ECHOK | ECHOE | ECHONL | ISIG | IEXTEN);
	newt.c_cc[VMIN] = 0;
	newt.c_cc[VTIME] = 1;
	tcsetattr(fileno(stdin), TCSANOW, &newt);

	// Get the current character
	ch = getchar();

	// Reapply old settings
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

	return ch;
}*/

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
    // if(motiontime>500 && motiontime<1000){
    //     cmd.mode = 1;
    //     cout<<"马上开始运动！"<<endl;
    // }

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
    //cout << "DEMO已经跑完！       ————njj" << endl;
    udp.SetSend(cmd);

    
    // while (1)
    // {
    //     int opt = 0;
		
    //     cout << "1为退出 2为向前 3未知 4未知" << endl;   

    //     cin >> opt;
        
    //     switch (opt)
	// 	{
	// 	case 1:
	// 		printf("already quit!\n");
    //         cmd.mode = 1;
    //         udp.SetSend(cmd);
	// 		exit(0);

	// 	case 2:
	// 		cmd.mode = 2;
    //         cmd.forwardSpeed=0.1f;
    //         sleep(100);
    //         udp.SetSend(cmd);
	// 		printf("move forward!\n");
            
	// 		break;

	// 	case 3:
	// 		cmd.mode = 1;
    //         cmd.roll = 0.5f;
    //         sleep(100);
    //         udp.SetSend(cmd);
	// 		printf("move backward!\n");
	// 		break;

	// 	case 4:
	// 		cmd.mode = 2;
    //         cmd.forwardSpeed=-0.1f;
    //         sleep(100);
    //         udp.SetSend(cmd);
	// 		printf("move left!\n");
	// 		break;

	// 	case 5:
			
	// 		printf("move right!\n");
	// 		break;

	// 	default:
	// 		printf("Stop!\n");
	// 		break;
		
    //     }
    // }
}
void Custom::KeyboardContol(){



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




    int opt = 0;
		
    cout << "1为扭转身体 2修复第一步，3为前进，4为低腰 ～～～～" << endl;    
    cin >> opt;
        if (opt == 1){
            cmd.mode = 1;
            cmd.roll = 0.5f;
            cout<<"111"<<endl;
        }
        else if (opt == 2){
            cmd.mode = 1;
            cmd.roll = -0.5f;       
        }
        else if (opt == 3){
            cmd.mode = 2;
            cmd.forwardSpeed=0.1f;
              
        }
        else if(opt == 4){
            cmd.mode = 1;
           cmd.bodyHeight = 0.3f;
        }
        else{
            cout<<"no!"<<endl;

        }
        
        /*switch (opt)
		{
		case 1:
            cmd.mode = 1;
            //udp.SetSend(cmd);
			printf("already quit!\n");
            exit(0);
           

		case 2:
			cmd.mode = 2;
            cmd.forwardSpeed=0.1f;
            sleep(100);
            udp.SetSend(cmd);
			printf("move forward!\n");
            
			break;

		case 3:
			cmd.mode = 1;
            cmd.roll = 0.5f;
            sleep(100);
            udp.SetSend(cmd);
			printf("move backward!\n");
			break;

		case 4:
			// cmd.mode = 2;
            // cmd.forwardSpeed=-0.1f;
            // sleep(100);
            // udp.SetSend(cmd);
			printf("move left!\n");
			break;

		case 5:
			cmd.mode = 0;
			printf("move right!\n");
			break;

		default:
			printf("Stop!\n");
			break;
}*/
        udp.SetSend(cmd);

}

int main(void) 
{
    std::cout << "Communication level is set to HIGH-level." << std::endl
              << "WARNING: Make sure the robot is standing on the ground." << std::endl
              << "Press Enter to continue..." << std::endl
              <<"!!!NJJ has changed the code!!!"<< std::endl;
              
    std::cin.ignore();

    Custom custom(HIGHLEVEL);
    // InitEnvironment();

    cout <<"输入1进入调试模式"<< endl;
    cout <<"输入2进入键盘模式"<< endl;

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

    else{
        cout<<"Stop"<<endl;
    }
        
        
      

    

    

    


    // while(1){
    //     sleep(10);
    // };

    return 0; 
}