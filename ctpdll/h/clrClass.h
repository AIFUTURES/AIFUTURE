#pragma once
#include <string>
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
#include <msclr\marshal_cppstd.h>  
#include <map>
using namespace std;
using namespace System;
using namespace System::Runtime::InteropServices;
using namespace msclr::interop;

#pragma comment (lib, "thostmduserapi.lib")  
#pragma comment (lib, "thosttraderapi.lib") 

vector < VariableRecord > VariRecord_Sum;	//在各个品种的表现
AccountRecord ARecord_Sum;					//记录账户的信息
vector < SteategyRecord > StraRecord_Sum;	//记录该账号上各个策略的表现情况
int VariableNum;
int ConnectFlag;


public ref class clrClass   //一定记得加public ref
{
public:
	clrClass(void);
	~clrClass(void);
	void initialize();
	void prepare();
	void update();
	void Check();
	void EClear();
	void exit();
	void Connect(int i);
	void CloseAccount(int i);
	void OpenTest();			//开仓测试
	void ClearAccount(int id);
	
	//账户信息
	int AccountNum();
	String^ AccountName(int i);
	double GetAveWin(int i);
	int GetDoneNum(int i);
	int GetUnDoneNum(int i);
	int GetDoneDeal(int i);
	double GetPositionProfit(int i);
	double GetCloseProfit(int i);
	double GetBalance(int i);
	double GetMargin(int i);
	int GetConnectFlag();
	int GetTrade(int i);
	double GetCommission(int i);

	//variable
	String^ GetVariID(int i, int j);
	int VariNum(int i);
	double GetVariAveWin(int i, int j);
	double GetVariPositionProfit(int i, int j);
	double GetVariCloseProfit(int i, int j);
	int GetVariPosition(int i, int j);
	double GetVariCommission(int i, int j);
	int GetVariAllOpen(int i, int j);
	int ClearVariable(int i, int j);
	//OrderRecord
	int GetOrderLength(int i);				//得到所有报单的数量
	String^  GetOrderVari(int i, int j);	//得到所有报单的品种
	String^  GetOrderTime(int i, int j);	//成交时间
	int GetOrderVol(int i, int j);			//成交数量
	
	double GetOrderPrice(int i, int j);		//成交价格
	String^ GetOrderType(int i, int j);		//成交类型
	String^ GetOrderStatu(int i, int j);

	//添加账户
	void AddInvesterID(String^ input) {
		string InvesterID = marshal_as<std::string>(input);
		monitor->AddInvesterID(InvesterID);
	}
	void AddPassWord(String^ input) {
		string PassWord = marshal_as<std::string>(input);
		monitor->AddPassWord(PassWord);
	}
	void AddBrokerID(String^ input) {
		string BrokerID = marshal_as<std::string>(input);
		monitor->AddBrokerID(BrokerID);
	}
	void AddTradeAddr(String^ input) {
		string TradeAddr = marshal_as<std::string>(input);
		monitor->AddTradeAddr(TradeAddr);
	}
	void AddProductInfo(String^ input) {
		string ProductInfo = marshal_as<std::string>(input);
		monitor->AddProduct(ProductInfo);
	}
	void AddAuthCode(String^ input) {
		string AuthCode = marshal_as<std::string>(input);
		monitor->AddAuthCode(AuthCode);
	}

	//资金线
	int SetMoneyDate(String^ begin, String^ end, int i);	//设置起始时间
	int GetMoneyLength(int i);				//得到资金线的点数
	void PrepareMoney(int i);				//读取资金数据
	String^ GetMoneyValue(int i, int j);
	String^ GetMoneyDate(int i, int j);

private:
	Monitor *monitor;
};