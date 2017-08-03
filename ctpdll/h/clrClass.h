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

vector < VariableRecord > VariRecord_Sum;	//�ڸ���Ʒ�ֵı���
AccountRecord ARecord_Sum;					//��¼�˻�����Ϣ
vector < SteategyRecord > StraRecord_Sum;	//��¼���˺��ϸ������Եı������
int VariableNum;
int ConnectFlag;


public ref class clrClass   //һ���ǵü�public ref
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
	void OpenTest();			//���ֲ���
	void ClearAccount(int id);
	
	//�˻���Ϣ
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
	int GetOrderLength(int i);				//�õ����б���������
	String^  GetOrderVari(int i, int j);	//�õ����б�����Ʒ��
	String^  GetOrderTime(int i, int j);	//�ɽ�ʱ��
	int GetOrderVol(int i, int j);			//�ɽ�����
	
	double GetOrderPrice(int i, int j);		//�ɽ��۸�
	String^ GetOrderType(int i, int j);		//�ɽ�����
	String^ GetOrderStatu(int i, int j);

	//����˻�
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

	//�ʽ���
	int SetMoneyDate(String^ begin, String^ end, int i);	//������ʼʱ��
	int GetMoneyLength(int i);				//�õ��ʽ��ߵĵ���
	void PrepareMoney(int i);				//��ȡ�ʽ�����
	String^ GetMoneyValue(int i, int j);
	String^ GetMoneyDate(int i, int j);

private:
	Monitor *monitor;
};