#include "stdafx.h"
#include <iostream>
#include <time.h>
#include <chrono>
#include <Windows.h>
#include <map>
#include "CustomMdSpi.h"
#include "CustomTradeSpi.h"


// ---- 全局参数声明 ---- //
extern TThostFtdcInstrumentIDType g_pTradeInstrumentID;       // 所交易的合约代码
extern TThostFtdcDirectionType gTradeDirection;               // 买卖方向
extern TThostFtdcPriceType gLimitPrice;                       // 交易价格
extern CustomMdSpi *pMdUserSpi;												  // 会话参数

extern vector < VariableRecord > VariRecord_Sum;		//在各个品种的表现
extern AccountRecord ARecord_Sum;						//记录账户的信息
extern vector < SteategyRecord > StraRecord_Sum;		//记录策略的运行情况
TThostFtdcFrontIDType	trade_front_id;	//前置编号
TThostFtdcSessionIDType	session_id;	//会话编号
TThostFtdcOrderRefType	order_ref;	//报单引用
time_t lOrderTime;
time_t lOrderOkTime;
//锁
HANDLE MoneyLineFlag;
extern HANDLE MutexAccount;
extern HANDLE MutexVariable;
extern HANDLE MutexStrategy;
extern map < string, TThostFtdcPriceType > MdHigh;
extern map < string, TThostFtdcPriceType > MdLow;



void CustomTradeSpi::Ref_Add() {
	cout << "orderref:  " << order_ref << endl;
	order_ref[11]++;
	for (int i = 11; i >= 0; ) {
		if (order_ref[i] >= '9') {
			order_ref[i] = '1';
			i--;
			order_ref[i]++;
		}
		else {
			break;
		}
	}
}

void CustomTradeSpi::OnFrontConnected()
{
	std::cout << "=====建立网络连接成功=====" << std::endl;
	ARecord.DoneNum = 0;		//盈利的手数
	ARecord.UnDoneNum = 0;		//亏损的手数
	EmergentFlag = 0;
	UpdateFlagVariable = 0;
	UpdateFlagAccount = 0;
	for (int i = 0; i < 12; i++)	order_ref[i] = '0';
	cout << "begin" << order_ref << endl;
	NowDay = "20170715";
	EndDay = "20170801";
	//客户认证
	if (AuthFlag == true) {
		CThostFtdcReqAuthenticateField *Confirm = new CThostFtdcReqAuthenticateField;
		strcpy(Confirm->UserID, gInvesterID);
		strcpy(Confirm->BrokerID, gBrokerID);
		strcpy(Confirm->UserProductInfo, gUserProductInfo);
		strcpy(Confirm->AuthCode, gAuthCode);
		int rt = g_pTradeUserApi->ReqAuthenticate(Confirm, 1);
		if (!rt) {
			std::cout << "投资者认证发送成功" << std::endl;
		}
		else {
			std::cout << "投资者认证发送失败" << std::endl;
		}
	}
	else
		reqUserLogin();
}

void CustomTradeSpi::OnRspAuthenticate(CThostFtdcRspAuthenticateField *pRspAuthenticateField, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	reqUserLogin();
	if (!isErrorRspInfo(pRspInfo)) {
		std::cout << "认证成功" << std::endl;
	}
}

void CustomTradeSpi::OnRspUserLogin(
	CThostFtdcRspUserLoginField *pRspUserLogin,
	CThostFtdcRspInfoField *pRspInfo,
	int nRequestID,
	bool bIsLast)
{
	if (!isErrorRspInfo(pRspInfo))
	{
		loginFlag = false;
		std::cout << "=====账户登录成功=====" << std::endl;
		std::cout << "交易日： " << pRspUserLogin->TradingDay << std::endl;
		std::cout << "登录时间： " << pRspUserLogin->LoginTime << std::endl;
		std::cout << "经纪商： " << pRspUserLogin->BrokerID << std::endl;
		std::cout << "帐户名： " << pRspUserLogin->UserID << std::endl;
		// 保存会话参数
		trade_front_id = pRspUserLogin->FrontID;
		session_id = pRspUserLogin->SessionID;
		//strcpy_s(order_ref, pRspUserLogin->MaxOrderRef);
		//reqQueryInstrument();
		// 投资者结算结果确认
		reqSettlementInfoConfirm();
	}
}

void CustomTradeSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	isErrorRspInfo(pRspInfo);
}

void CustomTradeSpi::OnFrontDisconnected(int nReason)
{
	std::cerr << "=====网络连接断开=====" << std::endl;
	std::cerr << "错误码： " << nReason << std::endl;
}

void CustomTradeSpi::OnHeartBeatWarning(int nTimeLapse)
{
	std::cerr << "=====网络心跳超时=====" << std::endl;
	std::cerr << "距上次连接时间： " << nTimeLapse << std::endl;
}

void CustomTradeSpi::OnRspUserLogout(
	CThostFtdcUserLogoutField *pUserLogout,
	CThostFtdcRspInfoField *pRspInfo,
	int nRequestID,
	bool bIsLast)
{
	if (!isErrorRspInfo(pRspInfo))
	{
		loginFlag = false; // 登出就不能再交易了 
		std::cout << "=====账户登出成功=====" << std::endl;
		std::cout << "经纪商： " << pUserLogout->BrokerID << std::endl;
		std::cout << "帐户名： " << pUserLogout->UserID << std::endl;
	}
}

void CustomTradeSpi::OnRspSettlementInfoConfirm(
	CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm,
	CThostFtdcRspInfoField *pRspInfo,
	int nRequestID,
	bool bIsLast)
{
	if (!isErrorRspInfo(pRspInfo))
	{
		std::cout << "=====投资者结算结果确认成功=====" << std::endl
			<< "账户： " << gInvesterID << std::endl
			<< "确认日期： " << pSettlementInfoConfirm->ConfirmDate << std::endl
			<< "确认时间： " << pSettlementInfoConfirm->ConfirmTime << std::endl;
		loginFlag = true;
	}
}

void CustomTradeSpi::OnRspQryInstrument(
	CThostFtdcInstrumentField *pInstrument,
	CThostFtdcRspInfoField *pRspInfo,
	int nRequestID,
	bool bIsLast)
{
	if (!isErrorRspInfo(pRspInfo))
	{
		std::cout << "=====查询合约结果成功=====" << std::endl;
		std::cout << "交易所代码： " << pInstrument->ExchangeID << std::endl;
		std::cout << "合约代码： " << pInstrument->InstrumentID << std::endl;
		std::cout << "合约在交易所的代码： " << pInstrument->ExchangeInstID << std::endl;
		std::cout << "执行价： " << pInstrument->StrikePrice << std::endl;
		std::cout << "到期日： " << pInstrument->EndDelivDate << std::endl;
		std::cout << "当前交易状态： " << pInstrument->IsTrading << std::endl;
	}
}

void CustomTradeSpi::OnRspQryTradingAccount(
	CThostFtdcTradingAccountField *pTradingAccount,
	CThostFtdcRspInfoField *pRspInfo,
	int nRequestID,
	bool bIsLast)
{
	if (!isErrorRspInfo(pRspInfo))
	{
		ARecord.Balance = pTradingAccount->Balance;					//期货结算准备金
		ARecord.Available = pTradingAccount->Available;				//可用资金
		ARecord.CurrMargin = pTradingAccount->CurrMargin;			//当前保证金
		ARecord.PositionProfit = pTradingAccount->PositionProfit;	//浮动盈亏
		ARecord.CloseProfit = pTradingAccount->CloseProfit;			//平仓盈亏
		ARecord.Commission = pTradingAccount->Commission;			//手续费
		ARecord.AveWin = ARecord.DoneDeal > 0 ? ARecord.CloseProfit / ARecord.DoneDeal : 0;	//平均每手盈利
																							//合并
		if (UpdateFlagAccount == 0)	return;
		WaitForSingleObject(MutexAccount, INFINITE);				//加锁
		UpdateFlagAccount = 0;
		ARecord_Sum.DoneNum += ARecord.DoneNum;
		ARecord_Sum.UnDoneNum += ARecord.UnDoneNum;
		ARecord_Sum.Available += ARecord.Available;
		ARecord_Sum.Balance += ARecord.Balance;
		ARecord_Sum.CurrMargin += ARecord.CurrMargin;
		ARecord_Sum.PositionProfit += ARecord.PositionProfit;
		ARecord_Sum.CloseProfit += ARecord.CloseProfit;
		ARecord_Sum.Commission += ARecord.Commission;
		ARecord_Sum.DoneDeal += ARecord.DoneDeal;
		ReleaseMutex(MutexAccount);									//解锁
	}
}

void CustomTradeSpi::OnRspQryInvestorPosition(
	CThostFtdcInvestorPositionField *pInvestorPosition,
	CThostFtdcRspInfoField *pRspInfo,
	int nRequestID,
	bool bIsLast)
{
	if (nRequestID != -1001)	return;
	if (!isErrorRspInfo(pRspInfo))
	{
		if (pInvestorPosition)
		{
			//if (pInvestorPosition->TodayPosition == 0)	return;		//只看那些有持仓的
			
			//std::cout << "=====查询投资者持仓成功=====" << std::endl;
			//std::cout << "合约代码： " << pInvestorPosition->InstrumentID << std::endl;
			//std::cout << "nRequestID： " << nRequestID << std::endl;
			//std::cout << "bIsLast： " << bIsLast << std::endl;
			//std::cout << "开仓价格： " << pInvestorPosition->UseMargin << std::endl;
			//std::cout << "开仓量： " << pInvestorPosition->OpenVolume << std::endl;
			//std::cout << "开仓方向： " << pInvestorPosition->PosiDirection << std::endl;    // 1净  2多  3空
			//std::cout << "今日持仓：" << pInvestorPosition->TodayPosition << std::endl;
			//std::cout << "持仓：" << pInvestorPosition->Position << std::endl;
			//std::cout << "浮动盈亏：" << pInvestorPosition->PositionProfit << std::endl;
			//std::cout << "平仓盈亏：" << pInvestorPosition->CloseProfit << std::endl;
			//std::cout << "手续费：" << pInvestorPosition->Commission << std::endl;
			//std::cout << "占用保证金：" << pInvestorPosition->OpenCost * pInvestorPosition->MarginRateByMoney << std::endl;
			bool HaveBeforePosition = pInvestorPosition->TodayPosition == 0 && pInvestorPosition->Position > 0;	//昨仓
			//单品种清仓
			string s = pInvestorPosition->InstrumentID;

			it = MdClear.find(s);
			if (it == MdClear.end()) {
				MdClear.insert(make_pair(s, 0.0));
			}
			
			it = MdHigh.find(s);
			char p[20];
			s.copy(p, s.length(), 0);
			p[s.length() ] = 0;
			if (it == MdHigh.end()) {
				char *g_pInstrumentID[] = { p };
				pMdUserSpi->registerMarketData(g_pInstrumentID);
			}

			VariableRecord tem;
			int pos = VariRecordPos(VariRecord, pInvestorPosition->InstrumentID);
			if (pos == -1) {
				pos = VariRecord.size();
				strcpy_s(tem.InstrumentID, pInvestorPosition->InstrumentID);
				VariRecord.push_back(tem);
			}
			VariableRecord &temr = VariRecord[pos];
			if (pInvestorPosition->PosiDirection == '2') {//买
				temr.OpenVolume_Buy += pInvestorPosition->OpenVolume;				//全部开仓	
				temr.TodayPosition_Buy += pInvestorPosition->Position;				//全部持仓
				//std::cout << "OpenPrice:  " << (pInvestorPosition->Position>0 ? pInvestorPosition->UseMargin / pInvestorPosition->Position : 0 )<< endl;
				temr.OpenPrice_Buy = pInvestorPosition->Position > 0 ? pInvestorPosition->UseMargin / pInvestorPosition->Position : 0;					//开仓均价
			}
			else if (pInvestorPosition->PosiDirection == '3') {//卖
				temr.OpenVolume_Sell += pInvestorPosition->OpenVolume;
				temr.TodayPosition_Sell += pInvestorPosition->Position;
				//std::cout << "OpenPrice:  " << (pInvestorPosition->Position>0 ? pInvestorPosition->UseMargin / pInvestorPosition->Position : 0) << endl;
				temr.OpenPrice_Sell = pInvestorPosition->Position>0 ? pInvestorPosition->UseMargin / pInvestorPosition->Position : 0;		//开仓均价
			}
			temr.CloseProfit += pInvestorPosition->CloseProfit;						//平仓盈亏
			temr.PositionProfit += pInvestorPosition->PositionProfit;				//浮动盈亏
			temr.Commission += pInvestorPosition->Commission;						//手续费
			temr.Margin += pInvestorPosition->OpenCost * pInvestorPosition->MarginRateByMoney;
			//紧急平仓或者单品种平仓
			
			if (( MdClear[s] > 0 && pInvestorPosition->Position > 0)) {
				string tems = pInvestorPosition->InstrumentID;
				it = MdHigh.find(tems);
				if (it == MdHigh.end())
					std::cout << ">>>>没有订阅当前品种的数据，无法平仓<<<<" << endl;
				else {
					std::cout << "紧急清仓: " << pInvestorPosition->InstrumentID << "  " << pInvestorPosition->PosiDirection << endl;
					TThostFtdcPriceType price;
					if (pInvestorPosition->PosiDirection == '2') { //卖出平仓
						price = MdLow[tems];
						if (!HaveBeforePosition) {
							if (MdClear[s] < 2) {
								std::cout << "已经平过了！" << std::endl;
								return;
							}
							MdClear[s] -= 2;
						}
						std::cout << "价格" << price << endl;
						if (pInvestorPosition->TodayPosition >0)
							reqOrderInsert(pInvestorPosition->InstrumentID, price, pInvestorPosition->TodayPosition, THOST_FTDC_D_Sell, THOST_FTDC_OF_CloseToday, THOST_FTDC_OPT_LimitPrice);
						if (pInvestorPosition->Position - pInvestorPosition->TodayPosition > 0)
							reqOrderInsert(pInvestorPosition->InstrumentID, price, pInvestorPosition->Position - pInvestorPosition->TodayPosition, THOST_FTDC_D_Sell, THOST_FTDC_OF_Close, THOST_FTDC_OPT_LimitPrice);
					}
					else if (pInvestorPosition->PosiDirection == '3') { //买入平仓
						price = MdHigh[tems];
						if (!HaveBeforePosition) {
							if (MdClear[s] != 1 && MdClear[s] != 3) {
								std::cout << "已经平过了！" << std::endl;
								return;
							}
							MdClear[s] -= 1;
						}
						std::cout << "价格" << price << endl;
						if (pInvestorPosition->TodayPosition >0)
							reqOrderInsert(pInvestorPosition->InstrumentID, price, pInvestorPosition->TodayPosition, THOST_FTDC_D_Buy, THOST_FTDC_OF_CloseToday, THOST_FTDC_OPT_LimitPrice);
						if (pInvestorPosition->Position - pInvestorPosition->TodayPosition > 0)
							reqOrderInsert(pInvestorPosition->InstrumentID, price, pInvestorPosition->Position - pInvestorPosition->TodayPosition, THOST_FTDC_D_Buy, THOST_FTDC_OF_Close, THOST_FTDC_OPT_LimitPrice);
					}
				}
			}
		}
	}
}

void CustomTradeSpi::OnRspOrderInsert(
	CThostFtdcInputOrderField *pInputOrder,
	CThostFtdcRspInfoField *pRspInfo,
	int nRequestID,
	bool bIsLast)
{

	if (!isErrorRspInfo(pRspInfo))
	{
		std::cout << "=====报单录入成功=====" << std::endl;
		std::cout << "合约代码： " << pInputOrder->InstrumentID << std::endl;
		std::cout << "价格： " << pInputOrder->LimitPrice << std::endl;
		std::cout << "数量： " << pInputOrder->VolumeTotalOriginal << std::endl;
		std::cout << "开仓方向： " << pInputOrder->Direction << std::endl;
		
	}
}

void CustomTradeSpi::OnRspOrderAction(
	CThostFtdcInputOrderActionField *pInputOrderAction,
	CThostFtdcRspInfoField *pRspInfo,
	int nRequestID,
	bool bIsLast)
{
	if (!isErrorRspInfo(pRspInfo))
	{
		/*std::cout << "=====报单操作成功=====" << std::endl;
		std::cout << "合约代码： " << pInputOrderAction->InstrumentID << std::endl;
		std::cout << "操作标志： " << pInputOrderAction->ActionFlag;*/
	}
}
/*
通过报单检查的报单指令（ReqOrderInsert），CTP后台会向客户端返回OnRtnOrder消息，其中OrderSubmitStatus为“已经提交”，
OrderStatus为“未知”。同时CTP后台将该报单指令转发至对应的交易所系统。交易所系统同样会对报单进行相应的检查，
如价格是否超出涨跌停板、报单指令是否试用等等，未通过交易所系统检查的报单，
CTP收到交易所系统响应后也会向客户端返回OnRtnOrder消息，其中OrderSubmitStatus为“报单已经被拒绝”，OrderStatus为“撤单”。
*/
void CustomTradeSpi::OnRtnOrder(CThostFtdcOrderField *pOrder)
{
	//	Sleep(1000);
	//sprintf(str, "%d", pOrder->OrderSubmitStatus);
	//报单状态0=已经提交，3=已经接受 4=被拒绝

										///全部成交 #define THOST_FTDC_OST_AllTraded '0'
										///部分成交还在队列中 #define THOST_FTDC_OST_PartTradedQueueing '1'
										///部分成交不在队列中 #define THOST_FTDC_OST_PartTradedNotQueueing '2' 
										///未成交还在队列中 #define THOST_FTDC_OST_NoTradeQueueing '3'
										///未成交不在队列中 #define THOST_FTDC_OST_NoTradeNotQueueing '4'
										///撤单 #define THOST_FTDC_OST_Canceled '5'
										///未知，表示Thost已经接受用户的委托指令，还没有转发到交易所 #define THOST_FTDC_OST_Unknown 'a'
										///尚未触发 #define THOST_FTDC_OST_NotTouched 'b'
										string SysID = pOrder->OrderSysID;
										std::cout << "=====收到报单应答=====" << std::endl;
										std::cout << "报单提交状态: " << pOrder->OrderSubmitStatus << std::endl;
										std::cout << "报单状态: " << pOrder->OrderStatus << std::endl;
										std::cout << "报单序号: " << pOrder->SequenceNo << std::endl;
										std::cout << "合约代码: " << pOrder->InstrumentID << std::endl;
										std::cout << "报单价格: " << pOrder->LimitPrice << std::endl;
										std::cout << "组合开平标志: " << pOrder->CombOffsetFlag[0] << std::endl;
										std::cout << "报单编号: " << pOrder->OrderSysID << std::endl;
										std::cout << "编号长度: " << SysID.length() << std::endl;
										std::cout << "量: " << pOrder->VolumeTotal << std::endl;
										
	if (pOrder->OrderSubmitStatus == '0' &&  pOrder->OrderStatus != '0' && pOrder->VolumeTotal > 0 && SysID.length() > 0) {
		OrderRecord TemOrder;
		strcpy_s(TemOrder.InstrumentID, pOrder->InstrumentID);
		TemOrder.Price = pOrder->LimitPrice;
		TemOrder.VolumeTotal = pOrder->VolumeTotal;
		TemOrder.Direction = pOrder->Direction;
		TemOrder.OffsetFlag = pOrder->CombOffsetFlag[0];
		strcpy_s(TemOrder.TradeTime, pOrder->InsertTime);
		TemOrder.status = "报单提交";
		OrderR.push_back(TemOrder);
	}

	if (isMyOrder(pOrder))
	{
		if (isTradingOrder(pOrder))
		{
			/*std::cout << "账户: " << gInvesterID << std::endl
			<< "--->>> 等待成交中！" << std::endl;*/
			//reqOrderAction(pOrder); // 这里可以撤单
			//reqUserLogout(); // 登出测试
		}
		else if (pOrder->OrderStatus == THOST_FTDC_OST_Canceled)
		{
			std::cout << "账户: " << gInvesterID << std::endl
				<< "--->>> 撤单成功！" << std::endl;
			ARecord.UnDoneNum += pOrder->VolumeTotal; 
			OrderRecord TemOrder;
			strcpy_s(TemOrder.InstrumentID, pOrder->InstrumentID);
			TemOrder.Price = pOrder->LimitPrice;
			TemOrder.VolumeTotal = pOrder->VolumeTotal;
			TemOrder.Direction = pOrder->Direction;
			TemOrder.OffsetFlag = pOrder->CombOffsetFlag[0];
			strcpy_s(TemOrder.TradeTime, pOrder->InsertTime);
			TemOrder.status = "撤单";
			OrderR.push_back(TemOrder);
		}
	}
}

void CustomTradeSpi::OnRtnTrade(CThostFtdcTradeField *pTrade)
{
	std::cout << "=====报单成功成交=====" << std::endl;
	std::cout << "成交时间： " << pTrade->TradeTime << std::endl;
	std::cout << "合约代码： " << pTrade->InstrumentID << std::endl;
	std::cout << "成交价格： " << pTrade->Price << std::endl;
	std::cout << "成交量： " << pTrade->Volume << std::endl;
	std::cout << "开平仓方向： " << pTrade->Direction << std::endl;
	std::cout << "开平标志： " << pTrade->OffsetFlag << std::endl;
	OrderRecord TemOrder;
	strcpy_s(TemOrder.InstrumentID, pTrade->InstrumentID);
	TemOrder.Price = pTrade->Price;
	TemOrder.VolumeTotal = pTrade->Volume;
	TemOrder.Direction = pTrade->Direction;
	TemOrder.OffsetFlag = pTrade->OffsetFlag;
	strcpy_s(TemOrder.TradeTime, pTrade->TradeTime);
	TemOrder.status = "成交";
	OrderR.push_back(TemOrder);
	int StrategyNum = 0;
	int pos = StraRecordPos(StraRecord, StrategyNum);
	if (pos == -1) {
		pos = StraRecord.size();
		SteategyRecord tem;
		tem.StrategyNum = StrategyNum;
		StraRecord.push_back(tem);
	}
	SteategyRecord &temsr = StraRecord[pos];
	int vpos = VariRecordPos(temsr.StrategyVariable, pTrade->InstrumentID);
	if (vpos == -1) {
		vpos = temsr.StrategyVariable.size();
		VariableRecord tem;
		strcpy_s(tem.InstrumentID, pTrade->InstrumentID);
		temsr.StrategyVariable.push_back(tem);
		//		std::cout << "test： " << vpos << std::endl;
	}
	VariableRecord &temvr = temsr.StrategyVariable[vpos];
	if (pTrade->Direction == THOST_FTDC_D_Buy) {
		if (pTrade->OffsetFlag == '0') {
			temvr.OpenPrice_Buy = (temvr.OpenPrice_Buy * temvr.TodayPosition_Buy + pTrade->Volume * pTrade->Price) / (temvr.TodayPosition_Buy + pTrade->Volume);

			temvr.OpenVolume_Buy += pTrade->Volume;
		}
		else if (pTrade->Volume <= temvr.TodayPosition_Sell) {
			temvr.CloseProfit += pTrade->Volume * (temvr.OpenPrice_Sell - pTrade->Price);
			temvr.TodayPosition_Sell -= pTrade->Volume;
			temvr.TodayPosition_Buy += pTrade->Volume;
		}
	}
	else {
		if (pTrade->OffsetFlag == '0') {
			temvr.OpenPrice_Sell = (temvr.OpenPrice_Sell * temvr.TodayPosition_Sell + pTrade->Volume * pTrade->Price) / (temvr.TodayPosition_Sell + pTrade->Volume);
			temvr.OpenVolume_Sell += pTrade->Volume;
		}
		else if (pTrade->Volume <= temvr.OpenPrice_Buy) {
			temvr.CloseProfit += pTrade->Volume * (pTrade->Price - temvr.OpenPrice_Buy);
			temvr.TodayPosition_Buy -= pTrade->Volume;
			temvr.TodayPosition_Sell += pTrade->Volume;
		}
	}

	//合并
	{
		WaitForSingleObject(MutexStrategy, INFINITE);				//加锁
		pos = StraRecordPos(StraRecord_Sum, StrategyNum);
		if (pos == -1) {
			pos = StraRecord_Sum.size();
			SteategyRecord tem;
			tem.StrategyNum = StrategyNum;
			StraRecord_Sum.push_back(tem);
		}
		SteategyRecord &temsr2 = StraRecord_Sum[pos];
		vpos = VariRecordPos(temsr2.StrategyVariable, pTrade->InstrumentID);
		if (vpos == -1) {
			vpos = temsr2.StrategyVariable.size();
			VariableRecord tem;
			strcpy_s(tem.InstrumentID, pTrade->InstrumentID);
			temsr2.StrategyVariable.push_back(tem);
		}
		VariableRecord &temvr = temsr2.StrategyVariable[vpos];
		if (pTrade->Direction == THOST_FTDC_D_Buy) {
			if (pTrade->OffsetFlag == '0') {
				temvr.OpenPrice_Buy = (temvr.OpenPrice_Buy * temvr.TodayPosition_Buy + pTrade->Volume * pTrade->Price) / (temvr.TodayPosition_Buy + pTrade->Volume);

				temvr.OpenVolume_Buy += pTrade->Volume;
			}
			else if (pTrade->Volume <= temvr.TodayPosition_Sell) {
				temvr.CloseProfit += pTrade->Volume * (temvr.OpenPrice_Sell - pTrade->Price);
				temvr.TodayPosition_Sell -= pTrade->Volume;
				temvr.TodayPosition_Buy += pTrade->Volume;
			}
		}
		else {
			if (pTrade->OffsetFlag == '0') {
				temvr.OpenPrice_Sell = (temvr.OpenPrice_Sell * temvr.TodayPosition_Sell + pTrade->Volume * pTrade->Price) / (temvr.TodayPosition_Sell + pTrade->Volume);
				temvr.OpenVolume_Sell += pTrade->Volume;
			}
			else if (pTrade->Volume <= temvr.OpenPrice_Buy) {
				temvr.CloseProfit += pTrade->Volume * (pTrade->Price - temvr.OpenPrice_Buy);
				temvr.TodayPosition_Buy -= pTrade->Volume;
				temvr.TodayPosition_Sell += pTrade->Volume;
			}
		}
		ReleaseMutex(MutexStrategy);									//解锁
	}


	ARecord.DoneNum += pTrade->Volume;
	if (pTrade->OffsetFlag != THOST_FTDC_OF_Open)	//只记录平仓，把成功平仓当作一次成功的操作
		ARecord.DoneDeal += pTrade->Volume;
}

bool CustomTradeSpi::isErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{
	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);
	if (bResult)
		std::cerr << "返回错误--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << std::endl;
	return bResult;
}

void CustomTradeSpi::reqUserLogin()
{
	CThostFtdcReqUserLoginField loginReq;
	memset(&loginReq, 0, sizeof(loginReq));
	strcpy_s(loginReq.BrokerID, gBrokerID);
	strcpy_s(loginReq.UserID, gInvesterID);
	strcpy_s(loginReq.Password, gInvesterPassword);
	static int requestID = 0; // 请求编号
	int rt = g_pTradeUserApi->ReqUserLogin(&loginReq, requestID);
	std::cout << "账户: " << gInvesterID << std::endl;
	if (!rt)
		std::cout << ">>>>>>发送登录请求成功" << std::endl;
	else
		std::cerr << "--->>>发送登录请求失败" << std::endl;
}

void CustomTradeSpi::reqUserLogout()
{
	CThostFtdcUserLogoutField logoutReq;
	memset(&logoutReq, 0, sizeof(logoutReq));
	strcpy_s(logoutReq.BrokerID, gBrokerID);
	strcpy_s(logoutReq.UserID, gInvesterID);
	static int requestID = 0; // 请求编号
	int rt = g_pTradeUserApi->ReqUserLogout(&logoutReq, requestID);
	if (!rt)
		std::cout << ">>>>>>发送登出请求成功" << std::endl;
	else
		std::cerr << "--->>>发送登出请求失败" << std::endl;
}


void CustomTradeSpi::reqSettlementInfoConfirm()
{
	CThostFtdcSettlementInfoConfirmField settlementConfirmReq;
	memset(&settlementConfirmReq, 0, sizeof(settlementConfirmReq));
	strcpy_s(settlementConfirmReq.BrokerID, gBrokerID);
	strcpy_s(settlementConfirmReq.InvestorID, gInvesterID);
	static int requestID = 0; // 请求编号
	int rt = g_pTradeUserApi->ReqSettlementInfoConfirm(&settlementConfirmReq, requestID);
	if (!rt)
	{
		std::cout << ">>>>>>发送投资者结算结果确认请求成功" << std::endl;
	}
	else
	{
		std::cerr << "--->>>发送投资者结算结果确认请求失败" << std::endl;
	}	
}


string StringPair(string search, TThostFtdcContentType content) {
	string result = "";
	int SearchLength = search.length();
	if (SearchLength == 0)	return "";
	for (int i = 0; i < 500; i++) {
		if (content[i] == 0)	return "";
		if (content[i] == search[0]) {
			bool find = true;
			for (int j = 0; j < SearchLength; j++) {
				if (search[j] != content[i + j]) {
					find = false;
					break;
				}
			}
			if (find == true) {
				for (int j = 0; ; j++) {
					if (content[i + SearchLength + j] == 0 || content[i + SearchLength + j] == '\n') {
						return result;
					}
					if(content[i + SearchLength + j] != ' ')
						result += content[i + SearchLength + j];
				}
			}
		}
	}
	return result;
}
//资金曲线
void CustomTradeSpi::GetMoneyLine() {
	static int requestID = 0;

	CThostFtdcQrySettlementInfoField pQrySettlementInfo;
	strcpy_s(pQrySettlementInfo.BrokerID, gBrokerID);
	strcpy_s(pQrySettlementInfo.InvestorID, gInvesterID);
	NowDay.copy(pQrySettlementInfo.TradingDay, NowDay.length(), 0);
	pQrySettlementInfo.TradingDay[8] = 0;
	int result = g_pTradeUserApi->ReqQrySettlementInfo(&pQrySettlementInfo, requestID++);
	if (!result)
		std::cout << ">>>>>>查询结算单成功  " << pQrySettlementInfo.TradingDay <<  std::endl;
	else
	{
		std::cout << ">>>>>>查询结算单失败  " << pQrySettlementInfo.TradingDay <<  std::endl;
		Sleep(500);
		GetMoneyLine();
	}
}

int CustomTradeSpi::reqMoneyLine() {
	if (NowDay < EndDay) {
		GetMoneyLine();
		NowDay = NextDay(NowDay);
		return 1;
	}
	return 0;
}
///投资者结算结果响应
void CustomTradeSpi::OnRspQrySettlementInfo(CThostFtdcSettlementInfoField *pSettlementInfo, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	if (!isErrorRspInfo(pRspInfo)) {
		if (pSettlementInfo == NULL) {
			std::cout << "投资者结算结果响应是空指针啊" << std::endl;
			return;
		}
		string FindResult = StringPair("Balance c/f：", pSettlementInfo->Content);
		if (FindResult.length() > 0)
		{
			std::cout << "查找结果:" << NowDay << "     " << FindResult << std::endl;
			Money  TemMoney;
			TemMoney.date = NowDay;
			TemMoney.money = FindResult;
			MoneyLine.push_back(TemMoney);
		}
		//std::cout << pSettlementInfo->Content << std::endl;		
	}
}


void CustomTradeSpi::reqQueryInstrument()
{
	if (loginFlag == false)	return;
	CThostFtdcQryInstrumentField instrumentReq;
	memset(&instrumentReq, 0, sizeof(instrumentReq));
	strcpy_s(instrumentReq.InstrumentID, g_pTradeInstrumentID);
	static int requestID = 0; // 请求编号
	VariRecord.clear();
	int rt = g_pTradeUserApi->ReqQryInstrument(&instrumentReq, requestID);
	/*if (!rt)
	std::cout << ">>>>>>发送合约查询请求成功" << std::endl;
	else
	std::cerr << "--->>>发送合约查询请求失败" << std::endl;*/
}

void CustomTradeSpi::reqQueryTradingAccount()
{
	if (loginFlag == false)	return;
	CThostFtdcQryTradingAccountField tradingAccountReq;
	memset(&tradingAccountReq, 0, sizeof(tradingAccountReq));
	strcpy_s(tradingAccountReq.BrokerID, gBrokerID);
	strcpy_s(tradingAccountReq.InvestorID, gInvesterID);
	static int requestID = 0; // 请求编号
	//std::this_thread::sleep_for(std::chrono::milliseconds(500)); // 有时候需要停顿一会才能查询成功
	Sleep(200);
	int rt = g_pTradeUserApi->ReqQryTradingAccount(&tradingAccountReq, requestID);
	if (!rt) {
		std::cout << ">>>>>>发送投资者资金账户查询请求成功" << std::endl;
	}
	else
	{
		std::cerr << "--->>>发送投资者资金账户查询请求失败,重新查询" << std::endl;
		reqQueryTradingAccount();
	}
}

void CustomTradeSpi::reqQueryInvestorPosition(TThostFtdcInstrumentIDType TradeInstrumentID, int time)
{
	if (loginFlag == false)	return;
	CThostFtdcQryInvestorPositionField postionReq;
	memset(&postionReq, 0, sizeof(postionReq));
	strcpy_s(postionReq.BrokerID, gBrokerID);
	strcpy_s(postionReq.InvestorID, gInvesterID);
	static int requestID = -1001; // 请求编号，用来标识这个请求是我这边发的，避免和另外一边的ctp发生混淆
	int rt = g_pTradeUserApi->ReqQryInvestorPosition(&postionReq, requestID);
	if (!rt) {
		std::cout << ">>>>>>发送投资者持仓查询请求成功" << std::endl;
		//UpdateFlagVariable = 1;
	}
	if (rt)
	{
		//这里要经常连续发送好几次，我也不知道为什么
		std::cerr << "--->>>发送投资者持仓查询请求失败,重新发送请求  " << rt << std::endl;
		Sleep(500);
		if (time < 10)	reqQueryInvestorPosition(g_pTradeInstrumentID, time++);
	}
	//reqMoneyLine();
}

void CustomTradeSpi::reqOrderInsert()
{
	
	CThostFtdcInputOrderField orderInsertReq;
	memset(&orderInsertReq, 0, sizeof(orderInsertReq));
	///经纪公司代码
	strcpy_s(orderInsertReq.BrokerID, gBrokerID);
	///投资者代码
	strcpy_s(orderInsertReq.InvestorID, gInvesterID);
	//用户代码
	strcpy_s(orderInsertReq.UserID, gInvesterID);
	///合约代码
	strcpy_s(orderInsertReq.InstrumentID, g_pTradeInstrumentID);
	///报单引用
	strcpy_s(orderInsertReq.OrderRef, order_ref);

	///报单价格条件: 限价
	orderInsertReq.OrderPriceType = THOST_FTDC_OPT_LimitPrice; //只能用limit
															   //有效期限
	orderInsertReq.TimeCondition = THOST_FTDC_TC_GTC;
	///买卖方向: 
	orderInsertReq.Direction = gTradeDirection;
	///组合开平标志: 开仓
	orderInsertReq.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
	///组合投机套保标志
	orderInsertReq.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
	///价格
	orderInsertReq.LimitPrice = gLimitPrice;
	///数量：1
	orderInsertReq.VolumeTotalOriginal = 1;
	///有效期类型: 当日有效
	orderInsertReq.TimeCondition = THOST_FTDC_TC_GFD;
	///成交量类型: 任何数量
	orderInsertReq.VolumeCondition = THOST_FTDC_VC_AV;
	///最小成交量: 1
	orderInsertReq.MinVolume = 0;
	///触发条件: 立即
	orderInsertReq.ContingentCondition = THOST_FTDC_CC_Immediately;
	///强平原因: 非强平
	orderInsertReq.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
	///自动挂起标志: 否
	orderInsertReq.IsAutoSuspend = 0;
	///用户强评标志: 否
	orderInsertReq.UserForceClose = 0;



	static int requestID = 0; // 请求编号
	int rt = g_pTradeUserApi->ReqOrderInsert(&orderInsertReq, ++requestID);
	if (!rt)
		std::cout << ">>>>>>发送报单录入请求成功" << std::endl;
	else
	{
		std::cerr << "--->>>发送报单录入请求失败,重新发送" << std::endl;

	}
}

void CustomTradeSpi::reqOrderInsert(
	TThostFtdcInstrumentIDType instrumentID,
	TThostFtdcPriceType price,
	TThostFtdcVolumeType volume,
	TThostFtdcDirectionType direction,
	TThostFtdcOffsetFlagType type,
	TThostFtdcOrderPriceTypeType PTypeType)
{
	Ref_Add();
	CThostFtdcInputOrderField orderInsertReq;
	memset(&orderInsertReq, 0, sizeof(orderInsertReq));
	///经纪公司代码
	strcpy_s(orderInsertReq.BrokerID, gBrokerID);
	///投资者代码
	strcpy_s(orderInsertReq.InvestorID, gInvesterID);
	///合约代码
	strcpy_s(orderInsertReq.InstrumentID, instrumentID);
	///报单引用
	strcpy_s(orderInsertReq.OrderRef, order_ref);
	///报单价格条件: 限价
	orderInsertReq.OrderPriceType = PTypeType;	//THOST_FTDC_OPT_AskPrice1
												///买卖方向: 
	orderInsertReq.Direction = direction;
	///组合开平标志: 开仓
	orderInsertReq.CombOffsetFlag[0] = type; //THOST_FTDC_OF_CloseToday;
											 ///组合投机套保标志
	orderInsertReq.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
	///价格
	orderInsertReq.LimitPrice = price;
	///数量：1
	orderInsertReq.VolumeTotalOriginal = volume;
	///有效期类型: 当日有效
	orderInsertReq.TimeCondition = THOST_FTDC_TC_GFD;
	///成交量类型: 任何数量
	orderInsertReq.VolumeCondition = THOST_FTDC_VC_AV;
	///最小成交量: 1
	orderInsertReq.MinVolume = 1;
	///触发条件: 立即
	orderInsertReq.ContingentCondition = THOST_FTDC_CC_Immediately;
	///强平原因: 非强平
	orderInsertReq.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
	///自动挂起标志: 否
	orderInsertReq.IsAutoSuspend = 0;
	///用户强平标志: 否
	orderInsertReq.UserForceClose = 0;

	static int requestID = 0; // 请求编号
	int rt = g_pTradeUserApi->ReqOrderInsert(&orderInsertReq, ++requestID);
	if (!rt)
		std::cout << ">>>>>>发送报单录入请求成功" << std::endl;
	else
		std::cerr << "--->>>发送报单录入请求失败" << std::endl;
}

void CustomTradeSpi::reqOrderAction(CThostFtdcOrderField *pOrder)
{
	static bool orderActionSentFlag = false; // 是否发送了报单
	if (orderActionSentFlag)
		return;

	CThostFtdcInputOrderActionField orderActionReq;
	memset(&orderActionReq, 0, sizeof(orderActionReq));
	///经纪公司代码
	strcpy_s(orderActionReq.BrokerID, pOrder->BrokerID);
	///投资者代码
	strcpy_s(orderActionReq.InvestorID, pOrder->InvestorID);
	///报单操作引用
	//	TThostFtdcOrderActionRefType	OrderActionRef;
	///报单引用
	strcpy_s(orderActionReq.OrderRef, pOrder->OrderRef);
	///请求编号
	//	TThostFtdcRequestIDType	RequestID;
	///前置编号
	orderActionReq.FrontID = trade_front_id;
	///会话编号
	orderActionReq.SessionID = session_id;
	///交易所代码
	//	TThostFtdcExchangeIDType	ExchangeID;
	///报单编号
	//	TThostFtdcOrderSysIDType	OrderSysID;
	///操作标志
	orderActionReq.ActionFlag = THOST_FTDC_AF_Delete;
	///价格
	//	TThostFtdcPriceType	LimitPrice;
	///数量变化
	//	TThostFtdcVolumeType	VolumeChange;
	///用户代码
	//	TThostFtdcUserIDType	UserID;
	///合约代码
	strcpy_s(orderActionReq.InstrumentID, pOrder->InstrumentID);
	static int requestID = 0; // 请求编号
	int rt = g_pTradeUserApi->ReqOrderAction(&orderActionReq, ++requestID);
	if (!rt)
		std::cout << ">>>>>>发送报单操作请求成功：撤单" << std::endl;
	else
		std::cerr << "--->>>发送报单操作请求失败：撤单" << std::endl;
	orderActionSentFlag = true;
}

bool CustomTradeSpi::isMyOrder(CThostFtdcOrderField *pOrder)
{
	//接收历史报文时，暂时都认为是自己的报文
	return true;
	//return ((pOrder->FrontID == trade_front_id) && (pOrder->SessionID == session_id) && (strcmp(pOrder->OrderRef, order_ref) == 0));
}

bool CustomTradeSpi::isTradingOrder(CThostFtdcOrderField *pOrder)
{
	return ((pOrder->OrderStatus != THOST_FTDC_OST_PartTradedNotQueueing) &&
		(pOrder->OrderStatus != THOST_FTDC_OST_Canceled) &&
		(pOrder->OrderStatus != THOST_FTDC_OST_AllTraded));
}

int CustomTradeSpi::VariRecordPos(vector < VariableRecord > VariRecords, TThostFtdcInstrumentIDType InstrumentID) {
	int pos = -1;
	int len = VariRecords.size();
	for (int i = 0; i < len; i++) {
		if (!strcmp(InstrumentID, VariRecords[i].InstrumentID)) {
			return i;
		}
	}
	return pos;
}

int CustomTradeSpi::StraRecordPos(vector < SteategyRecord > StraRecord, int strategynum) {
	int pos = -1;
	int len = StraRecord.size();
	for (int i = 0; i < len; i++) {
		if (StraRecord[i].StrategyNum == strategynum) {
			return i;
		}
	}
	return pos;
}

string CustomTradeSpi::NextDay(string day) {
	if (day.length() < 8)	return "";
	day[7] += 1;
	if (day[7] > '9') {	//个位数  日
		day[7] = '0';
		day[6] += 1;
	}
	if (day[6] >= '3' && day[7] > '2') {	//十位数  日
		day[5] += 1;
		day[6] = '0';
		day[7] = '1';
	}
	if (day[4] == '0'  && day[5] > '9') {	//个位数  月
		day[4] += 1;
		day[5] = '0';
	}
	if (day[4] == '1' && day[5] > '2') {
		day[4] = '0';
		day[5] = '1';
		day[3] += 1;
	}
	if (day[3] > '9') {
		day[3] = 0;
		day[2] += 1;
		if (day[2] > '9') {
			day[2] = '0';
			day[1] += 1;
			if (day[1] > '9') {
				day[1] = '0';
				day[0] += 1;
			}
		}
	}
	return day;
}
void CustomTradeSpi::Check() {
	//std::cout << std::endl << "=====投资者资金账户=====" << std::endl
	//	<< "投资者账号： " << gInvesterID << std::endl
	//	<< "可用资金： " << ARecord.Available << std::endl
	//	<< "期货结算准备金： " << ARecord.Balance << std::endl
	//	<< "当前保证金: " << ARecord.CurrMargin << std::endl
	//	<< "浮动盈亏： " << ARecord.PositionProfit << std::endl
	//	<< "平仓盈亏： " << ARecord.CloseProfit << std::endl
	//	<< "手续费： " << ARecord.Commission << std::endl
	//	<< "成功成交数量(平仓)： " << ARecord.DoneNum << std::endl		//完成平仓操作才认为是成功成功
	//	<< "无法成交总数量(撤单)： " << ARecord.UnDoneNum << std::endl
	//	<< "平均每手平仓盈利： " << ARecord.AveWin << std::endl;

	std::cout << std::endl << "=====各个品种运行情况=====" << std::endl;
	int len = VariRecord.size();
	for (int i = 0; i < len; i++) {
		VariableRecord temr = VariRecord[i];
		std::cout << "合约ID： " << temr.InstrumentID << std::endl
			<< "今日开多总数量： " << temr.OpenVolume_Buy << std::endl
			<< "今日开空总数量： " << temr.OpenVolume_Sell << std::endl
			<< "当前多头持仓： " << temr.TodayPosition_Buy << "   持仓均价： " << temr.OpenPrice_Buy << std::endl
			<< "当前空头持仓： " << temr.TodayPosition_Sell << "   持仓均价： " << temr.OpenPrice_Sell << std::endl
			<< "手续费： " << temr.Commission << std::endl
			<< "浮动盈亏： " << temr.PositionProfit << std::endl
			<< "平仓盈亏： " << temr.CloseProfit << std::endl
			<< "保证金： " << temr.Margin << std::endl
			<< "====================" << std::endl;
	}
	/*std::cout << std::endl << "=====各个策略运行情况=====" << StraRecord.size() << std::endl;
	for (int i = 0; i < StraRecord.size(); i++) {
		SteategyRecord temsr = StraRecord[i];
		std::cout << "Strategy Number: " << temsr.StrategyNum << endl;
		for (int j = 0; j < temsr.StrategyVariable.size(); j++) {
			int TradeVolume = (temsr.StrategyVariable[j].OpenVolume_Buy + temsr.StrategyVariable[j].OpenVolume_Sell);
			std::cout << "品种:  " << temsr.StrategyVariable[j].InstrumentID << endl
				<< "平均每手盈利:  " << (TradeVolume > 0 ? temsr.StrategyVariable[j].CloseProfit / TradeVolume : 0) << endl
				<< "开仓数量:  " << temsr.StrategyVariable[j].OpenVolume_Buy + temsr.StrategyVariable[j].OpenVolume_Sell << endl;
		}
	}*/
}

void CustomTradeSpi::SetPara(string broker, string invester, string password, CThostFtdcTraderApi* api, string addr, string UserProductInfo, string AuthCode) {
	int i = 0;
	for (i = 0; i < broker.length(); i++) gBrokerID[i] = broker[i];  gBrokerID[i] = 0;
	for (i = 0; i < invester.length(); i++) gInvesterID[i] = invester[i];  gInvesterID[i] = 0;
	for (i = 0; i < password.length(); i++) gInvesterPassword[i] = password[i];  gInvesterPassword[i] = 0;
	for (i = 0; i < addr.length(); i++) gTradeFrontAddr[i] = addr[i];  gTradeFrontAddr[i] = 0;
	//登陆认证
	if (UserProductInfo.length() > 0)	
	{
		AuthFlag = true;
		for (i = 0; i < UserProductInfo.length(); i++) gUserProductInfo[i] = UserProductInfo[i];  gUserProductInfo[i] = 0;
		for (i = 0; i < AuthCode.length(); i++) gAuthCode[i] = AuthCode[i];  gAuthCode[i] = 0;
	}
	g_pTradeUserApi = api;
}

void CustomTradeSpi::UpdateRecords() {
	VariRecord.clear();
	//	StraRecord.clear();
	UpdateFlagAccount = 1;
	UpdateFlagVariable = 1;
	AccountRecord tem;
	tem.DoneDeal = ARecord.DoneDeal;
	tem.DoneNum = ARecord.DoneNum;
	tem.UnDoneNum = ARecord.UnDoneNum;
	ARecord = tem;
	reqQueryTradingAccount();
	reqQueryInvestorPosition(g_pTradeInstrumentID, 0);
}