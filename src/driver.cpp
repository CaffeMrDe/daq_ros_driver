#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include <soem/ethercattype.h>
#include <soem/nicdrv.h>
#include <soem/ethercatbase.h>
#include <soem/ethercatmain.h>
#include <soem/ethercatdc.h>
#include <soem/ethercatcoe.h>
#include <soem/ethercatfoe.h>
#include <soem/ethercatconfig.h>
#include <soem/ethercatprint.h>

#include <ros/ros.h>
#include "geometry_msgs/WrenchStamped.h"

#define IFRAME "eth1"
#define DEBUG

using namespace std;

int main(int argc, char **argv){

	int ret;
	float *buffer;
	char IOmap[4096];
	int chk;


	ros::init(argc, argv, "daq_driver");
	ros::NodeHandle n;
	ros::Publisher state_pub;


	ret = ec_init(IFRAME);		// 初始化soem
	if(ret < 0){
		cout << "ec_init error, ret = " << ret << endl;
		return -1;
	}	

	ret = ec_config_init(FALSE);	// 初始化并配置
	if(ret < 0){
		cout << "ec_config_init error, ret = " << ret << endl;
		return -1;	
	}

	cout << "ec_slavecount = " << ec_slavecount << endl;
	if(ec_slavecount > 0)
		cout << "ec_slave0 name was: " << ec_slave[1].name << endl;

	ec_config_map(&IOmap);
	ec_configdc();

	ec_statecheck(0, EC_STATE_SAFE_OP,  EC_TIMEOUTSTATE);	// 等待所有从站切换至safe OP状态
	cout << "all slave state was safe op" << endl;

	ec_slave[0].state = EC_STATE_OPERATIONAL;
        ec_send_processdata();
        ec_receive_processdata(EC_TIMEOUTRET);
	ec_writestate(0);

	chk = 40;
        do
        {
            ec_send_processdata();
            ec_receive_processdata(EC_TIMEOUTRET);
            ec_statecheck(0, EC_STATE_OPERATIONAL, 50000);
        }
        while (chk-- && (ec_slave[0].state != EC_STATE_OPERATIONAL)); // 等待所有从站切换至OP状态
	cout << "all slave state was op" << endl;

#ifdef DEBUG
	cout << "+++++++++++++++++++++" << chk << endl;
	cout << "Obits" << ec_slave[1].Ibits << endl;
#endif


	ros::Rate loop_rate(20);
	state_pub = n.advertise<geometry_msgs::Wrench>("daq_data", 1000);
	geometry_msgs::Wrench tmp; 

	while(ros::ok()){

		ec_send_processdata();
		ret = ec_receive_processdata(EC_TIMEOUTRET);
		cout << "ec_receive_processdata ret = " << ret << endl;
		buffer = (float *)ec_slave[1].inputs;
#ifdef DEBUG
		cout << "Fx=" << buffer[0] << " || Fy= " << buffer[1] << " || Fz=" << buffer[2] << " || Tx=" << buffer[3] << " || Ty=" << buffer[4] << " || Tz=" << buffer[5] << endl;
#endif
		tmp.force.x = buffer[0];tmp.force.y = buffer[1];tmp.force.z = buffer[2];
		tmp.torque.x = buffer[3];tmp.torque.y = buffer[4];tmp.torque.z = buffer[5];
		state_pub.publish(tmp);

		loop_rate.sleep();
	}


	return 0;
}
