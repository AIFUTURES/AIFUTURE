#pragma once
// ---- 派生的交易类 ---- //
#include "ThostFtdcTraderApi.h"
#include <string>
#include <Vector>
#include <map>
using namespace std;

extern map < string, TThostFtdcPriceType > MdHigh;
extern map < string, TThostFtdcPriceType > MdLow;

//记录账户的表现
struct AccountRecord {
	///监控的参数
	int DoneNum = 0;						//成交总数量
	int UnDoneNum = 0;						//未能成交的总数量（撤单）
	int DoneDeal = 0;						//成交且平仓的手数
	TThostFtdcMoneyType AveWin = 0;			//平均每手盈利
	TThostFtdcMoneyType Available = 0;		//可用资金
	TThostFtdcMoneyType Balance = 0;		//期货结算准备金
	TThostFtdcMoneyType CurrMargin = 0;		//当前保证金
	TThostFtdcMoneyType PositionProfit = 0; //浮动盈亏
	TThostFtdcMoneyType CloseProfit = 0;	//平仓盈亏
	TThostFtdcMoneyType Commission = 0;		//手续费
};
//记录账户在各个品种上的表现
struct VariableRecord {
	TThostFtdcInstrumentIDType InstrumentID;		//合约ID
	TThostFtdcVolumeType OpenVolume_Buy = 0;		//开多总数量
	TThostFtdcVolumeType OpenVolume_Sell = 0;		//开空总数量
	TThostFtdcVolumeType TodayPosition_Buy = 0;		//当前多头持仓
	TThostFtdcVolumeType TodayPosition_Sell = 0;	//当前空头持仓
	TThostFtdcVolumeType TodayUndoneNum = 0;		//撤单数量
	TThostFtdcMoneyType OpenPrice_Buy = 0;			//多头均价
	TThostFtdcMoneyType OpenPrice_Sell = 0;			//空头均价
	TThostFtdcMoneyType Commission = 0;				//手续费
	TThostFtdcMoneyType CloseProfit = 0;			//平仓盈亏
	TThostFtdcMoneyType PositionProfit = 0;			//浮动盈亏
	TThostFtdcMoneyType Margin = 0;					//保证金

	//用来更新的变量，上一次查询的值
	TThostFtdcVolumeType LastTodayPosition_Buy = 0;		//当前多头持仓
	TThostFtdcVolumeType LastTodayPosition_Sell = 0;	//当前空头持仓
	TThostFtdcVolumeType LastOpenVolume_Buy = 0;		//开多总数量
	TThostFtdcVolumeType LastOpenVolume_Sell = 0;		//开空总数量
	TThostFtdcMoneyType LastCommissionBuy = 0;			//手续费,买
	TThostFtdcMoneyType LastCommissionSell = 0;			//手续费，卖
	TThostFtdcMoneyType LastCloseProfitBuy = 0;			//平仓盈亏
	TThostFtdcMoneyType LastCloseProfitSell = 0;		//平仓盈亏
	TThostFtdcMoneyType LastPositionProfitBuy = 0;		//浮动盈亏
	TThostFtdcMoneyType LastPositionProfitSell = 0;		//浮动盈亏
	TThostFtdcMoneyType LastMarginBuy = 0;				//保证金
	TThostFtdcMoneyType LastMarginSell = 0;				//保证金

};
//记录账户在各个策略的表现
struct SteategyRecord {
	int StrategyNum;
	vector< VariableRecord > StrategyVariable;
	TThostFtdcMoneyType CloseProfit = 0;			//平仓盈亏
	TThostFtdcVolumeType OpenVolume_Buy = 0;		//开多总数量
	TThostFtdcVolumeType OpenVolume_Sell = 0;		//开空总数量
};


//记录单
struct OrderRecord {
	TThostFtdcInstrumentIDType InstrumentID;		//合约ID
	TThostFtdcTimeType	TradeTime;					//成交时间
	TThostFtdcVolumeType VolumeTotal = 0;			//总数量
	TThostFtdcVolumeType VolumeTraded = 0;			//成交的量
	TThostFtdcMoneyType Price = 0;					//均价
	TThostFtdcDirectionType	Direction = 0;			//买卖方向
	TThostFtdcOffsetFlagType OffsetFlag = 0;		//开平标志
	string status="";
};

struct Money {
	string date;
	string money;
};

class CustomTradeSpi : public CThostFtdcTraderSpi
{
	// ---- ctp_api部分回调接口 ---- //
public:
	///当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
	void OnFrontConnected();

	//用户认证
	void OnRspAuthenticate(CThostFtdcRspAuthenticateField *pRspAuthenticateField, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) ;

	///登录请求响应
	void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///错误应答
	void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///当客户端与交易后台通信连接断开时，该方法被调用。当发生这个情况后，API会自动重新连接，客户端可不做处理。
	void OnFrontDisconnected(int nReason);

	///心跳超时警告。当长时间未收到报文时，该方法被调用。
	void OnHeartBeatWarning(int nTimeLapse);

	///登出请求响应
	void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///投资者结算结果响应
	void OnRspQrySettlementInfo(CThostFtdcSettlementInfoField *pSettlementInfo,CThostFtdcRspInfoField *pRspInfo,int nRequestID,bool bIsLast);

	///投资者结算结果确认响应
	void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///请求查询合约响应
	void OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///请求查询资金账户响应
	void OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///请求查询投资者持仓响应
	void OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///请求查询投资者持仓响应
	void OnRspQryInvestorPositionDetail(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///报单录入请求响应
	void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///报单操作请求响应
	void OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///报单通知
	void OnRtnOrder(CThostFtdcOrderField *pOrder);

	///成交通知
	void OnRtnTrade(CThostFtdcTradeField *pTrade);

	// ---- 自定义函数 ---- //
public:  //private
	bool loginFlag; // 登陆成功的标识
	void Check();	//输出各个品种的情况
	void Ref_Add(); //报单引用叠加规则
	void CustomTradeSpi::reqOrderInsert(
		TThostFtdcInstrumentIDType instrumentID,
		TThostFtdcPriceType price,
		TThostFtdcVolumeType volume,
		TThostFtdcDirectionType direction,
		TThostFtdcOffsetFlagType type,
		TThostFtdcOrderPriceTypeType PTypeType); // 个性化报单录入，外部调用

												 //监控参数
	std::vector < VariableRecord > VariRecord;	//记录该账号在各个品质上的表现情况
	std::vector < SteategyRecord > StraRecord;	//记录该账号上各个策略的表现情况
	std::vector < OrderRecord > OrderR;			//记录所有成交单或撤单
	std::vector < Money >	    MoneyLine;		//资金线
	AccountRecord ARecord;						//记录账户的信息
	int UpdateFlagAccount;
	int UpdateFlagVariable;
	int EmergentFlag;
	std::map<string, TThostFtdcPriceType>::iterator it;
	map < string, TThostFtdcPriceType > MdClear;

private:
	void reqUserLogin();				// 登录请求
	void reqUserLogout();				// 登出请求
	void reqSettlementInfoConfirm();	// 投资者结果确认
	void reqQueryInstrument();			// 请求查询合约
	void reqQueryTradingAccount();		// 请求查询资金帐户
	void reqQueryInvestorPosition(TThostFtdcInstrumentIDType g_pTradeInstrumentID, int time); // 请求查询投资者持仓
	void reqOrderInsert(); // 请求报单录入
	void reqOrderAction(CThostFtdcOrderField *pOrder);			// 请求报单操作
	bool isErrorRspInfo(CThostFtdcRspInfoField *pRspInfo);		// 是否收到错误信息
	bool isMyOrder(CThostFtdcOrderField *pOrder);				// 是否我的报单回报
	bool isTradingOrder(CThostFtdcOrderField *pOrder);			// 是否正在交易的报单
	string NextDay(string day);									// 得到下一天
	int VariRecordPos(vector < VariableRecord > VariRecord, TThostFtdcInstrumentIDType InstrumentID);	//寻找队列中是否已经存过对应的品种数据
	int StraRecordPos(vector < SteategyRecord > StraRecord, int strategynum);
	//登陆账号的参数
	TThostFtdcBrokerIDType gBrokerID;                      // 模拟经纪商代码
	TThostFtdcInvestorIDType gInvesterID;                  // 投资者账户名
	TThostFtdcPasswordType gInvesterPassword;              // 投资者密码
	CThostFtdcTraderApi *g_pTradeUserApi;                  // 交易指针
	///认证信息
	TThostFtdcProductInfoType	gUserProductInfo;
	TThostFtdcAuthCodeType	gAuthCode;
	bool AuthFlag;
public:

	char gTradeFrontAddr[50];                                // 模拟交易前置地址

	string NowDay;											//查询资金线开始的时间
	string EndDay;											//查询资金线结束的时间
	//设置该交易线程的参数
	void SetPara(string broker, string invester, string password, CThostFtdcTraderApi* api, string addr, string UserProductInfo, string AuthCode);
	int reqMoneyLine();
	//得到该账户的资金曲线
	void GetMoneyLine();
	void UpdateRecords();
	char* GetId() {
		return gInvesterID;
	}
	void ClearAll() {
		it = MdClear.begin();
		while (it != MdClear.end())
		{
			it->second = 3;
			it++;
		}
	}
	int ClearVariable(string s) {
		it = MdClear.find(s);
		if (it != MdClear.end()) {
			MdClear[s] = 3;
			return 1;
		}
		else
			return 0;
	}
	//下单测试
	void OpenTest() {
		if (loginFlag == false)	return;
		TThostFtdcInstrumentIDType g_pTradeInstrumentID = "rb1710";
		string tems = g_pTradeInstrumentID;
		TThostFtdcPriceType price = MdHigh[tems];
		std::map<string, TThostFtdcPriceType>::iterator itt;
		itt = MdHigh.find(tems);
		if (itt == MdHigh.end())	//如果没有当前品种的最高价，则无法交易
			return;
		reqOrderInsert(g_pTradeInstrumentID, price, 1, THOST_FTDC_D_Buy, THOST_FTDC_OF_Open, THOST_FTDC_OPT_LimitPrice); // 自定义一笔交易

		TThostFtdcInstrumentIDType g_pTradeInstrumentID2 = "cu1710";
		tems = g_pTradeInstrumentID2;
		price = MdHigh[tems];
		itt = MdHigh.find(tems);
		if (itt == MdHigh.end())	//如果没有当前品种的最高价，则无法交易
			return;
		reqOrderInsert(g_pTradeInstrumentID2, price, 1, THOST_FTDC_D_Buy, THOST_FTDC_OF_Open, THOST_FTDC_OPT_LimitPrice); // 自定义一笔交易

		TThostFtdcInstrumentIDType g_pTradeInstrumentID3 = "i1801";
		tems = g_pTradeInstrumentID3;
		price = MdHigh[tems];
		itt = MdHigh.find(tems);
		if (itt == MdHigh.end())	//如果没有当前品种的最高价，则无法交易
			return;
		reqOrderInsert(g_pTradeInstrumentID3, price, 1, THOST_FTDC_D_Buy, THOST_FTDC_OF_Open, THOST_FTDC_OPT_LimitPrice); // 自定义一笔交易

		TThostFtdcInstrumentIDType g_pTradeInstrumentID4 = "IF1712";
		tems = g_pTradeInstrumentID4;
		price = MdHigh[tems];
		itt = MdHigh.find(tems);
		if (itt == MdHigh.end())	//如果没有当前品种的最高价，则无法交易
			return;
		reqOrderInsert(g_pTradeInstrumentID4, price, 1, THOST_FTDC_D_Buy, THOST_FTDC_OF_Open, THOST_FTDC_OPT_LimitPrice); // 自定义一笔交易

	}
};

