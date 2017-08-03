#include "stdafx.h"
#include <stdio.h>
#include <windows.h>
#include <iostream>
#include "ThostFtdcTraderApi.h"
#include "ThostFtdcMdApi.h"
#include "ThostFtdcUserApiDataType.h"
#include "ThostFtdcUserApiStruct.h"
#include "CustomMdSpi.h"
#include "CustomTradeSpi.h"
#include "Monitor.h"
#include "time.h"
using namespace std;

#pragma comment (lib, "thostmduserapi.lib")  
#pragma comment (lib, "thosttraderapi.lib") 


// ---- 全局变量 ---- //  

//simow
TThostFtdcBrokerIDType gBrokerID = "9999";                         // 模拟经纪商代码  
TThostFtdcInvestorIDType gInvesterID = "083370";                         // 投资者账户名  
TThostFtdcPasswordType gInvesterPassword = "013579";                     // 投资者密码  

// 行情参数  
CThostFtdcMdApi *g_pMdUserApi = nullptr;                           // 行情指针  
char gMdFrontAddr[] = "tcp://180.168.146.187:10010";               // 模拟行情前置地址
//char gMdFrontAddr[] = "tcp://180.168.146.187:10031";               // 非开盘时间
// 交易参数  
CThostFtdcTraderApi *g_pTradeUserApi = nullptr;                    // 交易指针  
char gTradeFrontAddr[] = "tcp://180.168.146.187:10000 ";            // 模拟交易前置地址  
//char gTradeFrontAddr[] = "tcp://180.168.146.187:10030";            // 非开盘时间
TThostFtdcInstrumentIDType g_pTradeInstrumentID = "Cu1710";         // 所交易的合约代码  
TThostFtdcDirectionType gTradeDirection = THOST_FTDC_D_Buy;       // 买卖方向  
TThostFtdcPriceType gLimitPrice = 3400;                            // 交易价格

char *g_pInstrumentID[] = { "rb1710" };                     // 行情合约代码列表
int instrumentNum = 1;                                             // 行情合约订阅数量 

string getTime()
{
	time_t timep;
	time(&timep);
	char tmp[64];
	strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", localtime(&timep));
	return tmp;
}

int main()
{
	// 账号密码  


	Monitor a;
	a.Prepare();
	a.Connect();
	while (1) {
		a.UpdateRelateAccount();
		cout << "===========" << getTime() << "===========" << endl;
		a.Check();
	}
	
	cout << "结束了" << endl;
	getchar();
	return 0;
}