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
	char iframe[1024];
	string siframe = "eth0";
	float buffer[6];
	char IOmap[4096];

	int psize = sizeof(buffer);

	cout << "sizeof(buffer) = " << sizeof(buffer) << endl;

	strncpy(iframe, siframe.c_str(), 1024);

	ret = ec_init(iframe);			// 初始化soem
	cout << "ret = " << ret << endl; 
    perror("--------------");

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
	ec_send_processdata();
	ret = ec_receive_processdata(EC_TIMEOUTRET);
	cout << "ret = " << ret << endl;

	ec_writestate(0);											// 修改从站状态
	ec_statecheck(0, EC_STATE_OPERATIONAL,  EC_TIMEOUTSTATE);	// 等待所有从站切换至OP状态
	cout << "all slave state was op" << endl;

	ret = ec_TxPDO(1, 0x6000, &psize, buffer, EC_TIMEOUTRXM);
	cout << "ec_TxPDO ret = " << ret << endl;

	cout << "buffer[0] = " << buffer[0] << endl;

	sleep(10);
	return 0;
}
