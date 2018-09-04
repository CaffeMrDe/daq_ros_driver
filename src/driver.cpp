#include <iostream>
#include <unistd.h>
#define NIC_DEBUG  1
#include <soem/ethercattype.h>
#include <soem/nicdrv.h>
#include <soem/ethercatbase.h>
#include <soem/ethercatmain.h>
#include <soem/ethercatdc.h>
#include <soem/ethercatcoe.h>
#include <soem/ethercatfoe.h>
#include <soem/ethercatconfig.h>
#include <soem/ethercatprint.h>
#include<stdio.h>
#include<string.h>

#define IFRAME "eth1"

using namespace std;

int main(){

	int ret;
	float *buffer;
	char IOmap[4096];
	int chk;

	ret = ec_init(IFRAME);			// 初始化soem
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

	ec_statecheck(0, EC_STATE_SAFE_OP,  EC_TIMEOUTSTATE);	// 等待所有从站切换至OP状态
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
        while (chk-- && (ec_slave[0].state != EC_STATE_OPERATIONAL));

	cout << "+++++++++++++++++++++" << chk << endl;
	cout << "Obits" << ec_slave[1].Ibits << endl;
 

	while(1){
		ec_send_processdata();
		ret = ec_receive_processdata(EC_TIMEOUTRET);
		cout << "ec_receive_processdata ret = " << ret << endl;
		buffer = (float *)ec_slave[1].inputs;
		cout << "Fx=" << buffer[0] << " || Fy= " << buffer[1] << " || Fz=" << buffer[2] << " || Tx=" << buffer[3] << " || Ty=" << buffer[4] << " || Tz=" << buffer[5] << endl;
		usleep(100000);
	}


	return 0;
}
