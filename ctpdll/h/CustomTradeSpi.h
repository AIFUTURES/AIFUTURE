#pragma once
// ---- �����Ľ����� ---- //
#include "ThostFtdcTraderApi.h"
#include <string>
#include <Vector>
#include <map>
using namespace std;

extern map < string, TThostFtdcPriceType > MdHigh;
extern map < string, TThostFtdcPriceType > MdLow;

//��¼�˻��ı���
struct AccountRecord {
	///��صĲ���
	int DoneNum = 0;						//�ɽ�������
	int UnDoneNum = 0;						//δ�ܳɽ�����������������
	int DoneDeal = 0;						//�ɽ���ƽ�ֵ�����
	TThostFtdcMoneyType AveWin = 0;			//ƽ��ÿ��ӯ��
	TThostFtdcMoneyType Available = 0;		//�����ʽ�
	TThostFtdcMoneyType Balance = 0;		//�ڻ�����׼����
	TThostFtdcMoneyType CurrMargin = 0;		//��ǰ��֤��
	TThostFtdcMoneyType PositionProfit = 0; //����ӯ��
	TThostFtdcMoneyType CloseProfit = 0;	//ƽ��ӯ��
	TThostFtdcMoneyType Commission = 0;		//������
};
//��¼�˻��ڸ���Ʒ���ϵı���
struct VariableRecord {
	TThostFtdcInstrumentIDType InstrumentID;		//��ԼID
	TThostFtdcVolumeType OpenVolume_Buy = 0;		//����������
	TThostFtdcVolumeType OpenVolume_Sell = 0;		//����������
	TThostFtdcVolumeType TodayPosition_Buy = 0;		//��ǰ��ͷ�ֲ�
	TThostFtdcVolumeType TodayPosition_Sell = 0;	//��ǰ��ͷ�ֲ�
	TThostFtdcVolumeType TodayUndoneNum = 0;		//��������
	TThostFtdcMoneyType OpenPrice_Buy = 0;			//��ͷ����
	TThostFtdcMoneyType OpenPrice_Sell = 0;			//��ͷ����
	TThostFtdcMoneyType Commission = 0;				//������
	TThostFtdcMoneyType CloseProfit = 0;			//ƽ��ӯ��
	TThostFtdcMoneyType PositionProfit = 0;			//����ӯ��
	TThostFtdcMoneyType Margin = 0;					//��֤��

	//�������µı�������һ�β�ѯ��ֵ
	TThostFtdcVolumeType LastTodayPosition_Buy = 0;		//��ǰ��ͷ�ֲ�
	TThostFtdcVolumeType LastTodayPosition_Sell = 0;	//��ǰ��ͷ�ֲ�
	TThostFtdcVolumeType LastOpenVolume_Buy = 0;		//����������
	TThostFtdcVolumeType LastOpenVolume_Sell = 0;		//����������
	TThostFtdcMoneyType LastCommissionBuy = 0;			//������,��
	TThostFtdcMoneyType LastCommissionSell = 0;			//�����ѣ���
	TThostFtdcMoneyType LastCloseProfitBuy = 0;			//ƽ��ӯ��
	TThostFtdcMoneyType LastCloseProfitSell = 0;		//ƽ��ӯ��
	TThostFtdcMoneyType LastPositionProfitBuy = 0;		//����ӯ��
	TThostFtdcMoneyType LastPositionProfitSell = 0;		//����ӯ��
	TThostFtdcMoneyType LastMarginBuy = 0;				//��֤��
	TThostFtdcMoneyType LastMarginSell = 0;				//��֤��

};
//��¼�˻��ڸ������Եı���
struct SteategyRecord {
	int StrategyNum;
	vector< VariableRecord > StrategyVariable;
	TThostFtdcMoneyType CloseProfit = 0;			//ƽ��ӯ��
	TThostFtdcVolumeType OpenVolume_Buy = 0;		//����������
	TThostFtdcVolumeType OpenVolume_Sell = 0;		//����������
};


//��¼��
struct OrderRecord {
	TThostFtdcInstrumentIDType InstrumentID;		//��ԼID
	TThostFtdcTimeType	TradeTime;					//�ɽ�ʱ��
	TThostFtdcVolumeType VolumeTotal = 0;			//������
	TThostFtdcVolumeType VolumeTraded = 0;			//�ɽ�����
	TThostFtdcMoneyType Price = 0;					//����
	TThostFtdcDirectionType	Direction = 0;			//��������
	TThostFtdcOffsetFlagType OffsetFlag = 0;		//��ƽ��־
	string status="";
};

struct Money {
	string date;
	string money;
};

class CustomTradeSpi : public CThostFtdcTraderSpi
{
	// ---- ctp_api���ֻص��ӿ� ---- //
public:
	///���ͻ����뽻�׺�̨������ͨ������ʱ����δ��¼ǰ�����÷��������á�
	void OnFrontConnected();

	//�û���֤
	void OnRspAuthenticate(CThostFtdcRspAuthenticateField *pRspAuthenticateField, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) ;

	///��¼������Ӧ
	void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///����Ӧ��
	void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///���ͻ����뽻�׺�̨ͨ�����ӶϿ�ʱ���÷��������á���������������API���Զ��������ӣ��ͻ��˿ɲ�������
	void OnFrontDisconnected(int nReason);

	///������ʱ���档����ʱ��δ�յ�����ʱ���÷��������á�
	void OnHeartBeatWarning(int nTimeLapse);

	///�ǳ�������Ӧ
	void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///Ͷ���߽�������Ӧ
	void OnRspQrySettlementInfo(CThostFtdcSettlementInfoField *pSettlementInfo,CThostFtdcRspInfoField *pRspInfo,int nRequestID,bool bIsLast);

	///Ͷ���߽�����ȷ����Ӧ
	void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///�����ѯ��Լ��Ӧ
	void OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///�����ѯ�ʽ��˻���Ӧ
	void OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///�����ѯͶ���ֲ߳���Ӧ
	void OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///�����ѯͶ���ֲ߳���Ӧ
	void OnRspQryInvestorPositionDetail(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///����¼��������Ӧ
	void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///��������������Ӧ
	void OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///����֪ͨ
	void OnRtnOrder(CThostFtdcOrderField *pOrder);

	///�ɽ�֪ͨ
	void OnRtnTrade(CThostFtdcTradeField *pTrade);

	// ---- �Զ��庯�� ---- //
public:  //private
	bool loginFlag; // ��½�ɹ��ı�ʶ
	void Check();	//�������Ʒ�ֵ����
	void Ref_Add(); //�������õ��ӹ���
	void CustomTradeSpi::reqOrderInsert(
		TThostFtdcInstrumentIDType instrumentID,
		TThostFtdcPriceType price,
		TThostFtdcVolumeType volume,
		TThostFtdcDirectionType direction,
		TThostFtdcOffsetFlagType type,
		TThostFtdcOrderPriceTypeType PTypeType); // ���Ի�����¼�룬�ⲿ����

												 //��ز���
	std::vector < VariableRecord > VariRecord;	//��¼���˺��ڸ���Ʒ���ϵı������
	std::vector < SteategyRecord > StraRecord;	//��¼���˺��ϸ������Եı������
	std::vector < OrderRecord > OrderR;			//��¼���гɽ����򳷵�
	std::vector < Money >	    MoneyLine;		//�ʽ���
	AccountRecord ARecord;						//��¼�˻�����Ϣ
	int UpdateFlagAccount;
	int UpdateFlagVariable;
	int EmergentFlag;
	std::map<string, TThostFtdcPriceType>::iterator it;
	map < string, TThostFtdcPriceType > MdClear;

private:
	void reqUserLogin();				// ��¼����
	void reqUserLogout();				// �ǳ�����
	void reqSettlementInfoConfirm();	// Ͷ���߽��ȷ��
	void reqQueryInstrument();			// �����ѯ��Լ
	void reqQueryTradingAccount();		// �����ѯ�ʽ��ʻ�
	void reqQueryInvestorPosition(TThostFtdcInstrumentIDType g_pTradeInstrumentID, int time); // �����ѯͶ���ֲ߳�
	void reqOrderInsert(); // ���󱨵�¼��
	void reqOrderAction(CThostFtdcOrderField *pOrder);			// ���󱨵�����
	bool isErrorRspInfo(CThostFtdcRspInfoField *pRspInfo);		// �Ƿ��յ�������Ϣ
	bool isMyOrder(CThostFtdcOrderField *pOrder);				// �Ƿ��ҵı����ر�
	bool isTradingOrder(CThostFtdcOrderField *pOrder);			// �Ƿ����ڽ��׵ı���
	string NextDay(string day);									// �õ���һ��
	int VariRecordPos(vector < VariableRecord > VariRecord, TThostFtdcInstrumentIDType InstrumentID);	//Ѱ�Ҷ������Ƿ��Ѿ������Ӧ��Ʒ������
	int StraRecordPos(vector < SteategyRecord > StraRecord, int strategynum);
	//��½�˺ŵĲ���
	TThostFtdcBrokerIDType gBrokerID;                      // ģ�⾭���̴���
	TThostFtdcInvestorIDType gInvesterID;                  // Ͷ�����˻���
	TThostFtdcPasswordType gInvesterPassword;              // Ͷ��������
	CThostFtdcTraderApi *g_pTradeUserApi;                  // ����ָ��
	///��֤��Ϣ
	TThostFtdcProductInfoType	gUserProductInfo;
	TThostFtdcAuthCodeType	gAuthCode;
	bool AuthFlag;
public:

	char gTradeFrontAddr[50];                                // ģ�⽻��ǰ�õ�ַ

	string NowDay;											//��ѯ�ʽ��߿�ʼ��ʱ��
	string EndDay;											//��ѯ�ʽ��߽�����ʱ��
	//���øý����̵߳Ĳ���
	void SetPara(string broker, string invester, string password, CThostFtdcTraderApi* api, string addr, string UserProductInfo, string AuthCode);
	int reqMoneyLine();
	//�õ����˻����ʽ�����
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
	//�µ�����
	void OpenTest() {
		if (loginFlag == false)	return;
		TThostFtdcInstrumentIDType g_pTradeInstrumentID = "rb1710";
		string tems = g_pTradeInstrumentID;
		TThostFtdcPriceType price = MdHigh[tems];
		std::map<string, TThostFtdcPriceType>::iterator itt;
		itt = MdHigh.find(tems);
		if (itt == MdHigh.end())	//���û�е�ǰƷ�ֵ���߼ۣ����޷�����
			return;
		reqOrderInsert(g_pTradeInstrumentID, price, 1, THOST_FTDC_D_Buy, THOST_FTDC_OF_Open, THOST_FTDC_OPT_LimitPrice); // �Զ���һ�ʽ���

		TThostFtdcInstrumentIDType g_pTradeInstrumentID2 = "cu1710";
		tems = g_pTradeInstrumentID2;
		price = MdHigh[tems];
		itt = MdHigh.find(tems);
		if (itt == MdHigh.end())	//���û�е�ǰƷ�ֵ���߼ۣ����޷�����
			return;
		reqOrderInsert(g_pTradeInstrumentID2, price, 1, THOST_FTDC_D_Buy, THOST_FTDC_OF_Open, THOST_FTDC_OPT_LimitPrice); // �Զ���һ�ʽ���

		TThostFtdcInstrumentIDType g_pTradeInstrumentID3 = "i1801";
		tems = g_pTradeInstrumentID3;
		price = MdHigh[tems];
		itt = MdHigh.find(tems);
		if (itt == MdHigh.end())	//���û�е�ǰƷ�ֵ���߼ۣ����޷�����
			return;
		reqOrderInsert(g_pTradeInstrumentID3, price, 1, THOST_FTDC_D_Buy, THOST_FTDC_OF_Open, THOST_FTDC_OPT_LimitPrice); // �Զ���һ�ʽ���

		TThostFtdcInstrumentIDType g_pTradeInstrumentID4 = "IF1712";
		tems = g_pTradeInstrumentID4;
		price = MdHigh[tems];
		itt = MdHigh.find(tems);
		if (itt == MdHigh.end())	//���û�е�ǰƷ�ֵ���߼ۣ����޷�����
			return;
		reqOrderInsert(g_pTradeInstrumentID4, price, 1, THOST_FTDC_D_Buy, THOST_FTDC_OF_Open, THOST_FTDC_OPT_LimitPrice); // �Զ���һ�ʽ���

	}
};

