#pragma once
#include <vector>
#include <string>
#include <map>
#include "ThostFtdcTraderApi.h"
#include "ThostFtdcMdApi.h"
#include "ThostFtdcUserApiDataType.h"
#include "ThostFtdcUserApiStruct.h"
#include "CustomMdSpi.h"
#include "CustomTradeSpi.h"

extern vector < VariableRecord > VariRecord_Sum;		//在各个品种的表现
extern AccountRecord ARecord_Sum;						//记录账户的信息
extern vector < SteategyRecord > StraRecord_Sum;		//记录策略的运行情况


extern HANDLE MutexAccount;
extern HANDLE MutexVariable;
extern HANDLE MutexStrategy;
extern HANDLE UpdateFlag;

using namespace std;
class Monitor {
private:


public:
	//监控分析
	int VariRecordPos(vector < VariableRecord > VariRecord_Sum, TThostFtdcInstrumentIDType InstrumentID);	//寻找队列中是否已经存过对应的品种数据

	vector < string > InvesterID;
	vector < string > PassWord;
	vector < string > BrokerID;
	vector < string > TradeAddr;
	vector < string > ProductID;
	vector < string > AuthCode;
	vector < CustomTradeSpi* > pTradeUserSpiVector;
	vector < CThostFtdcTraderApi* > pTradeUserApiVector;

	map < string, int > InvesterIDMap;

public:
	void CombineVariRecord();
	//void CombineAccountRecord();
	void Prepare();
	void Connect();
	void Connect(int i);

	void UpdateRelateAccount();
	void Check();
	void Clear();
	void Resume();

	void OpenTest() {
		for (int i = 0; i < pTradeUserSpiVector.size(); i++)
			pTradeUserSpiVector[i]->OpenTest();
	}
	//资金账户信息
	char* GetId(int i) {
		if (i == -1)  return "汇总账户";
		return pTradeUserSpiVector[i]->GetId();
	}
	int AccountNum() {
		return pTradeUserSpiVector.size();
	}
	double GetAveWin(int i) {
		if (i == -1)  return ARecord_Sum.AveWin;
		return pTradeUserSpiVector[i]->ARecord.AveWin;
	}
	int GetDoneNum(int i) {
		if (i == -1)  return ARecord_Sum.DoneNum;
		return pTradeUserSpiVector[i]->ARecord.DoneNum;
	}
	int GetUnDoneNum(int i) {
		if (i == -1)  return ARecord_Sum.UnDoneNum;
		return pTradeUserSpiVector[i]->ARecord.UnDoneNum;
	}
	int GetDoneDeal(int i) {
		if (i == -1)  return ARecord_Sum.DoneDeal;
		return pTradeUserSpiVector[i]->ARecord.DoneDeal;
	}
	double GetPositionProfit(int i) {
		if (i == -1)  return ARecord_Sum.PositionProfit;
		return pTradeUserSpiVector[i]->ARecord.PositionProfit;
	}
	double GetCloseProfit(int i) {
		if (i == -1)  return ARecord_Sum.CloseProfit;
		return pTradeUserSpiVector[i]->ARecord.CloseProfit;
	}
	double GetBalance(int i) {
		if (i == -1)  return ARecord_Sum.Balance;
		return pTradeUserSpiVector[i]->ARecord.Balance;
	}
	double GetMargin(int i) {
		if (i == -1)  return ARecord_Sum.CurrMargin;
		return pTradeUserSpiVector[i]->ARecord.CurrMargin;
	}
	double GetCommission(int i) {
		if (i == -1)  return ARecord_Sum.Commission;
		return pTradeUserSpiVector[i]->ARecord.Commission;
	}

	//各个品种的信息
	char* GetVariID(int i, int j) {
		if (i == -1)  return VariRecord_Sum[j].InstrumentID;
		return pTradeUserSpiVector[i]->VariRecord[j].InstrumentID;
	}
	int VariNum(int i) {
		if (i == -1)  return VariRecord_Sum.size();
		return pTradeUserSpiVector[i]->VariRecord.size();
	}
	double GetVariAveWin(int i, int j) {
		return 0.0;
	}
	
	double GetVariPositionProfit(int i, int j) {
		if (i == -1) {
			WaitForSingleObject(UpdateFlag, INFINITE);
			double result = VariRecord_Sum[j].PositionProfit;
			ReleaseMutex(UpdateFlag);
			return result;
		}
		return pTradeUserSpiVector[i]->VariRecord[j].PositionProfit;
	}
	double GetVariCloseProfit(int i, int j) {
		if (i == -1) {
			WaitForSingleObject(UpdateFlag, INFINITE);
			double result =  VariRecord_Sum[j].CloseProfit;
			ReleaseMutex(UpdateFlag);
			return result;
		}
		return pTradeUserSpiVector[i]->VariRecord[j].CloseProfit;
	}
	double GetVariCommission(int i, int j) {
		if (i == -1) {
			WaitForSingleObject(UpdateFlag, INFINITE);
			double result = VariRecord_Sum[j].Commission;
			ReleaseMutex(UpdateFlag);
			return result;
		}
		return pTradeUserSpiVector[i]->VariRecord[j].Commission;
	}
	int GetVariAllOpen(int i, int j) {
		if (i == -1) {
			WaitForSingleObject(UpdateFlag, INFINITE);
			int result = VariRecord_Sum[j].OpenVolume_Buy + VariRecord_Sum[j].OpenVolume_Sell;
			ReleaseMutex(UpdateFlag);
			return result;
		}
		return pTradeUserSpiVector[i]->VariRecord[j].OpenVolume_Buy + pTradeUserSpiVector[i]->VariRecord[j].OpenVolume_Sell;
	}
	int GetVariPosition(int i, int j) {
		if (i == -1) {
			WaitForSingleObject(UpdateFlag, INFINITE);
			int result = VariRecord_Sum[j].TodayPosition_Buy - VariRecord_Sum[j].TodayPosition_Sell;
			ReleaseMutex(UpdateFlag);
			return result;
		}
		return (int)(pTradeUserSpiVector[i]->VariRecord[j].TodayPosition_Buy - pTradeUserSpiVector[i]->VariRecord[j].TodayPosition_Sell);
	}

	//添加账户
	void AddInvesterID(string input) {
		//cout << input << endl;
		InvesterID.push_back(input);
	}
	void AddPassWord(string input) {
		//cout << input << endl;
		PassWord.push_back(input);
	}
	void AddBrokerID(string input) {
		//cout << input << endl;
		BrokerID.push_back(input);
	}
	void AddTradeAddr(string input) {
		//cout << input << endl;
		TradeAddr.push_back(input);
	}
	void AddProduct(string input) {
		ProductID.push_back(input);
	}
	void AddAuthCode(string input) {
		AuthCode.push_back(input);
	}
};