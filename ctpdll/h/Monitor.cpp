#pragma once
#include "stdafx.h"
#include <iostream>
#include <Windows.h>
#include "Monitor.h"

extern vector < VariableRecord > VariRecord_Sum;		//在各个品种的表现
extern AccountRecord ARecord_Sum;						//记录账户的信息
extern vector < SteategyRecord > StraRecord_Sum;		//记录策略的运行情况

// 行情参数  
CThostFtdcMdApi *g_pMdUserApi = nullptr;                           // 行情指针  
CustomMdSpi *pMdUserSpi;
char gMdFrontAddr[] = "tcp://180.168.146.187:10010";               // 模拟行情前置地址  10  31
char *g_pInstrumentID[] = { "rb1710" ,"cu1710" ,"cu1709","i1801","IF1712" };        // 行情合约代码列表
int instrumentNum = 5;                                             // 行情合约订阅数量 		
TThostFtdcBrokerIDType gBrokerID = "9999";                         // 模拟经纪商代码  
TThostFtdcInvestorIDType gInvesterID = "083370";                         // 投资者账户名  
TThostFtdcPasswordType gInvesterPassword = "013579";                     // 投资者密码  


HANDLE MutexAccount;
HANDLE MutexVariable;
HANDLE MutexStrategy;
HANDLE UpdateFlag;
// 交易参数  
TThostFtdcInstrumentIDType g_pTradeInstrumentID = "cu1710";         // 所交易的合约代码  
TThostFtdcDirectionType gTradeDirection = THOST_FTDC_D_Buy;         // 买卖方向  
TThostFtdcPriceType gLimitPrice = 3400;                             // 交易价格
//存交易品种的涨停、跌停价，清仓时用到
map < string, TThostFtdcPriceType > MdHigh;
map < string, TThostFtdcPriceType > MdLow;
extern int ConnectFlag;

void Monitor::Prepare() {
	std::cout << "=====准备信息中=====" << endl;
	////账号1
	//InvesterID.push_back("083370");
	//PassWord.push_back("013579");
	//BrokerID.push_back("9999");
	//TradeAddr.push_back("tcp://180.168.146.187:10000");
	////账号2
	//InvesterID.push_back("097128");
	//PassWord.push_back("013579");
	//BrokerID.push_back("9999");
	//TradeAddr.push_back("tcp://180.168.146.187:10000");

	/*int len = InvesterID.size();
	if (PassWord.size() != len || BrokerID.size() != len || TradeAddr.size() != len) {
		std::cout << "准备的数据不能对齐，请检查是否有疏漏" << endl;
		return;
	}
	for (int i = 0; i < InvesterID.size(); i++) {
		InvesterIDMap.insert(make_pair(InvesterID[i], i));
	}*/

	cout << "初始化行情..." << endl;
	ConnectFlag = 0;
	g_pMdUserApi = CThostFtdcMdApi::CreateFtdcMdApi();   // 创建行情实例  
	pMdUserSpi = new CustomMdSpi;       // 创建行情回调实例  
	g_pMdUserApi->RegisterSpi(pMdUserSpi);               // 注册事件类  
	g_pMdUserApi->RegisterFront(gMdFrontAddr);           // 设置行情前置地址  
	g_pMdUserApi->Init();                                // 连接运行  
														 /*g_pMdUserApi->Join();
														 delete pMdUserSpi;
														 g_pMdUserApi->Release();*/
	MutexAccount = CreateMutex(NULL, FALSE, NULL);
	MutexVariable = CreateMutex(NULL, FALSE, NULL);
	MutexStrategy = CreateMutex(NULL, FALSE, NULL);
	UpdateFlag = CreateMutex(NULL, FALSE, NULL);

}

void Monitor::Connect() {
	int len = InvesterID.size();
	for (int i = 0; i < len; i++) {
		CThostFtdcTraderApi *g_pTradeUserApi = nullptr;
		cout << "初始化交易..." << endl;
		g_pTradeUserApi = CThostFtdcTraderApi::CreateFtdcTraderApi(); // 创建交易实例   
		CustomTradeSpi *pTradeSpi = new CustomTradeSpi;               // 创建交易回调实例  
		pTradeSpi->SetPara(BrokerID[i], InvesterID[i], PassWord[i], g_pTradeUserApi, TradeAddr[i], ProductID[i], AuthCode[i]);
		g_pTradeUserApi->RegisterSpi(pTradeSpi);                      // 注册事件类  
		g_pTradeUserApi->SubscribePublicTopic(THOST_TERT_RESTART);    // 订阅公共流,只接受登陆后的内容
		g_pTradeUserApi->SubscribePrivateTopic(THOST_TERT_RESTART);   // 订阅私有流  
		g_pTradeUserApi->RegisterFront(pTradeSpi->gTradeFrontAddr);   // 设置交易前置地址  
		pTradeUserSpiVector.push_back(pTradeSpi);
		pTradeUserApiVector.push_back(g_pTradeUserApi);
		g_pTradeUserApi->Init();                                      // 连接运行  

	}
	Sleep(1000);
}
void Monitor::Connect(int i) {
	InvesterIDMap.insert(make_pair(InvesterID[i], i));
	int len = InvesterID.size();
	CThostFtdcTraderApi *g_pTradeUserApi = nullptr;
	cout << "初始化交易..." << endl;
	g_pTradeUserApi = CThostFtdcTraderApi::CreateFtdcTraderApi(); // 创建交易实例   
	CustomTradeSpi *pTradeSpi = new CustomTradeSpi;               // 创建交易回调实例  
	pTradeSpi->SetPara(BrokerID[i], InvesterID[i], PassWord[i], g_pTradeUserApi, TradeAddr[i], ProductID[i], AuthCode[i]);
	g_pTradeUserApi->RegisterSpi(pTradeSpi);                      // 注册事件类  
	g_pTradeUserApi->SubscribePublicTopic(THOST_TERT_RESTART);    // 订阅公共流,只接受登陆后的内容
	g_pTradeUserApi->SubscribePrivateTopic(THOST_TERT_RESTART);   // 订阅私有流  
	g_pTradeUserApi->RegisterFront(pTradeSpi->gTradeFrontAddr);   // 设置交易前置地址  
	pTradeUserSpiVector.push_back(pTradeSpi);
	pTradeUserApiVector.push_back(g_pTradeUserApi);
	g_pTradeUserApi->Init();                                      // 连接运行  
	Sleep(1000);
}


//void Monitor::CombineAccountRecord() {
//	AccountRecord newAR;
//	ARecord_Sum = newAR;
//	for (int i = 0; i < pTradeUserSpiVector.size(); i++) {
//		AccountRecord temr = pTradeUserSpiVector[i]->ARecord;
//		ARecord_Sum.DoneNum += temr.DoneNum;
//		ARecord_Sum.UnDoneNum += temr.UnDoneNum;
//		ARecord_Sum.Available += temr.Available;
//		ARecord_Sum.Balance += temr.Balance;
//		ARecord_Sum.CurrMargin += temr.CurrMargin;
//		ARecord_Sum.PositionProfit += temr.PositionProfit;
//		ARecord_Sum.CloseProfit += temr.CloseProfit;
//		ARecord_Sum.Commission += temr.Commission;
//		ARecord_Sum.DoneDeal += temr.DoneDeal;
//	}
//	ARecord_Sum.AveWin = ARecord_Sum.DoneDeal > 0 ? ARecord_Sum.CloseProfit / ARecord_Sum.DoneDeal : 0;
//}


void Monitor::CombineVariRecord() {
	WaitForSingleObject(MutexVariable, INFINITE);							//加锁
	VariRecord_Sum.clear();
	for (int i = 0; i < pTradeUserSpiVector.size(); i++) {
		vector < VariableRecord > temvari = pTradeUserSpiVector[i]->VariRecord;
		for (int j = 0; j < temvari.size(); j++) {
			VariableRecord tem = temvari[j];
			int pos = VariRecordPos(VariRecord_Sum, tem.InstrumentID);
			if (pos == -1) {
				VariableRecord tempos;
				pos = VariRecord_Sum.size();
				strcpy_s(tempos.InstrumentID, tem.InstrumentID);
				VariRecord_Sum.push_back(tempos);
			}
			VariableRecord &temr = VariRecord_Sum[pos];
			temr.OpenVolume_Buy += tem.OpenVolume_Buy;									//开多总数量
			temr.OpenVolume_Sell += tem.OpenVolume_Sell;								//开空总数量
			temr.TodayPosition_Buy += tem.TodayPosition_Buy;							//当前多头持仓
			temr.TodayPosition_Sell += tem.TodayPosition_Sell;							//当前空头持仓
			temr.OpenPrice_Buy += tem.OpenPrice_Buy*tem.TodayPosition_Buy;				//多头均价
			temr.OpenPrice_Sell += tem.OpenPrice_Sell*tem.TodayPosition_Sell;			//空头均价
			temr.CloseProfit += tem.CloseProfit;										//平仓盈亏
			temr.PositionProfit += tem.PositionProfit;									//浮动盈亏
			temr.Commission += tem.Commission;											//手续费
			temr.Margin += tem.Margin;													//保证金
		}
	}
	for (int i = 0; i < VariRecord_Sum.size(); i++) {
		VariRecord_Sum[i].OpenPrice_Buy = VariRecord_Sum[i].TodayPosition_Buy > 0 ? VariRecord_Sum[i].OpenPrice_Buy / VariRecord_Sum[i].TodayPosition_Buy : 0;
		VariRecord_Sum[i].OpenPrice_Sell = VariRecord_Sum[i].TodayPosition_Sell>0 ? VariRecord_Sum[i].OpenPrice_Sell / VariRecord_Sum[i].TodayPosition_Sell : 0;
	}
	ReleaseMutex(MutexVariable);
}

int Monitor::VariRecordPos(vector < VariableRecord > VariRecord_Sum, TThostFtdcInstrumentIDType InstrumentID) {
	int pos = -1;
	int len = VariRecord_Sum.size();
	for (int i = 0; i < len; i++) {
		if (!strcmp(InstrumentID, VariRecord_Sum[i].InstrumentID)) {
			return i;
		}
	}
	return pos;
}

void Monitor::Clear() {
	for (int i = 0; i < pTradeUserSpiVector.size(); i++) {
		pTradeUserSpiVector[i]->EmergentFlag = 1;
		pTradeUserSpiVector[i]->ClearAll();
	}
}

void Monitor::Resume() {
	for (int i = 0; i < pTradeUserSpiVector.size(); i++) {
		pTradeUserSpiVector[i]->EmergentFlag = 0;
	}
}

void Monitor::UpdateRelateAccount() {
	std::cout << "更新相关数据" << std::endl;
	//数据重置
	WaitForSingleObject(UpdateFlag, INFINITE);				//加锁
	VariRecord_Sum.clear();
	AccountRecord newAR; ARecord_Sum = newAR;
	for (int i = 0; i < pTradeUserSpiVector.size(); i++) {
		pTradeUserSpiVector[i]->UpdateRecords();
	}
	Sleep(1000);	
	//合并好之后算一下平均盈亏、持仓均价什么的
	ARecord_Sum.AveWin = ARecord_Sum.DoneDeal > 0 ? ARecord_Sum.CloseProfit / ARecord_Sum.DoneDeal : 0;
	for (int i = 0; i < VariRecord_Sum.size(); i++) {
		VariRecord_Sum[i].OpenPrice_Buy = VariRecord_Sum[i].TodayPosition_Buy > 0 ? VariRecord_Sum[i].OpenPrice_Buy / VariRecord_Sum[i].TodayPosition_Buy : 0;
		VariRecord_Sum[i].OpenPrice_Sell = VariRecord_Sum[i].TodayPosition_Sell>0 ? VariRecord_Sum[i].OpenPrice_Sell / VariRecord_Sum[i].TodayPosition_Sell : 0;
	}
	CombineVariRecord();
	ReleaseMutex(UpdateFlag);
}

void Monitor::Check() {
	for (int i = 0; i < pTradeUserSpiVector.size(); i++)	pTradeUserSpiVector[i]->Check();
	std::cout << std::endl << "=====汇总资金账户=====" << std::endl
		<< "可用资金： " << ARecord_Sum.Available << std::endl
		<< "期货结算准备金： " << ARecord_Sum.Balance << std::endl
		<< "当前保证金: " << ARecord_Sum.CurrMargin << std::endl
		<< "浮动盈亏： " << ARecord_Sum.PositionProfit << std::endl
		<< "平仓盈亏： " << ARecord_Sum.CloseProfit << std::endl
		<< "手续费： " << ARecord_Sum.Commission << std::endl
		<< "成功成交数量： " << ARecord_Sum.DoneNum << std::endl
		<< "成功平仓数量： " << ARecord_Sum.DoneDeal << std::endl
		<< "撤单数量：     " << ARecord_Sum.UnDoneNum << std::endl
		<< "平均每手平仓盈利： " << ARecord_Sum.AveWin << std::endl;

	std::cout << std::endl << "=====各个品种运行情况=====" << std::endl
		<< "====================" << endl;
	int len = VariRecord_Sum.size();
	for (int i = 0; i < len; i++) {
		VariableRecord temr = VariRecord_Sum[i];
		std::cout << "====================" << std::endl
			<< "合约ID： " << temr.InstrumentID << std::endl
			<< "今日开多总数量： " << temr.OpenVolume_Buy << std::endl
			<< "今日开空总数量： " << temr.OpenVolume_Sell << std::endl
			<< "当前多头持仓： " << temr.TodayPosition_Buy << "   持仓均价： " << temr.OpenPrice_Buy << std::endl
			<< "当前空头持仓： " << temr.TodayPosition_Sell << "   持仓均价： " << temr.OpenPrice_Sell << std::endl
			<< "手续费： " << temr.Commission << std::endl
			<< "平仓盈亏： " << temr.PositionProfit << std::endl
			<< "浮动盈亏： " << temr.CloseProfit << std::endl
			<< "保证金： " << temr.Margin << std::endl
			<< "====================" << std::endl;
	}
	/*std::cout << std::endl << "=====各个策略运行情况=====" << std::endl;
	for (int i = 0; i < StraRecord_Sum.size(); i++) {
		SteategyRecord temsr = StraRecord_Sum[i];
		std::cout << "Strategy Number: " << temsr.StrategyNum << endl;
		for (int j = 0; j < temsr.StrategyVariable.size(); j++) {
			int TradeVolume = (temsr.StrategyVariable[j].OpenVolume_Buy + temsr.StrategyVariable[j].OpenVolume_Sell);
			std::cout << "品种:  " << temsr.StrategyVariable[j].InstrumentID << endl
				<< "平均每手盈利:  " << (TradeVolume > 0 ? temsr.StrategyVariable[j].CloseProfit / TradeVolume : 0) << endl
				<< "开仓数量:  " << temsr.StrategyVariable[j].OpenVolume_Buy + temsr.StrategyVariable[j].OpenVolume_Sell << endl;
		}
	}*/
}