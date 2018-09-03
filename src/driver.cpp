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

	ec_TxPDO(1, 0x6000, &psize, buffer, 100);
	cout << "psize = " << psize << endl;

	sleep(10);
	return 0;
}
