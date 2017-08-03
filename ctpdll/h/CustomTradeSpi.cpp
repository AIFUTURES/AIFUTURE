#include "stdafx.h"
#include <iostream>
#include <time.h>
#include <chrono>
#include <Windows.h>
#include <map>
#include "CustomMdSpi.h"
#include "CustomTradeSpi.h"


// ---- ȫ�ֲ������� ---- //
extern TThostFtdcInstrumentIDType g_pTradeInstrumentID;       // �����׵ĺ�Լ����
extern TThostFtdcDirectionType gTradeDirection;               // ��������
extern TThostFtdcPriceType gLimitPrice;                       // ���׼۸�
extern CustomMdSpi *pMdUserSpi;												  // �Ự����

extern vector < VariableRecord > VariRecord_Sum;		//�ڸ���Ʒ�ֵı���
extern AccountRecord ARecord_Sum;						//��¼�˻�����Ϣ
extern vector < SteategyRecord > StraRecord_Sum;		//��¼���Ե��������
TThostFtdcFrontIDType	trade_front_id;	//ǰ�ñ��
TThostFtdcSessionIDType	session_id;	//�Ự���
TThostFtdcOrderRefType	order_ref;	//��������
time_t lOrderTime;
time_t lOrderOkTime;
//��
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
	std::cout << "=====�����������ӳɹ�=====" << std::endl;
	ARecord.DoneNum = 0;		//ӯ��������
	ARecord.UnDoneNum = 0;		//���������
	EmergentFlag = 0;
	UpdateFlagVariable = 0;
	UpdateFlagAccount = 0;
	for (int i = 0; i < 12; i++)	order_ref[i] = '0';
	cout << "begin" << order_ref << endl;
	NowDay = "20170715";
	EndDay = "20170801";
	//�ͻ���֤
	if (AuthFlag == true) {
		CThostFtdcReqAuthenticateField *Confirm = new CThostFtdcReqAuthenticateField;
		strcpy(Confirm->UserID, gInvesterID);
		strcpy(Confirm->BrokerID, gBrokerID);
		strcpy(Confirm->UserProductInfo, gUserProductInfo);
		strcpy(Confirm->AuthCode, gAuthCode);
		int rt = g_pTradeUserApi->ReqAuthenticate(Confirm, 1);
		if (!rt) {
			std::cout << "Ͷ������֤���ͳɹ�" << std::endl;
		}
		else {
			std::cout << "Ͷ������֤����ʧ��" << std::endl;
		}
	}
	else
		reqUserLogin();
}

void CustomTradeSpi::OnRspAuthenticate(CThostFtdcRspAuthenticateField *pRspAuthenticateField, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	reqUserLogin();
	if (!isErrorRspInfo(pRspInfo)) {
		std::cout << "��֤�ɹ�" << std::endl;
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
		std::cout << "=====�˻���¼�ɹ�=====" << std::endl;
		std::cout << "�����գ� " << pRspUserLogin->TradingDay << std::endl;
		std::cout << "��¼ʱ�䣺 " << pRspUserLogin->LoginTime << std::endl;
		std::cout << "�����̣� " << pRspUserLogin->BrokerID << std::endl;
		std::cout << "�ʻ����� " << pRspUserLogin->UserID << std::endl;
		// ����Ự����
		trade_front_id = pRspUserLogin->FrontID;
		session_id = pRspUserLogin->SessionID;
		//strcpy_s(order_ref, pRspUserLogin->MaxOrderRef);
		//reqQueryInstrument();
		// Ͷ���߽�����ȷ��
		reqSettlementInfoConfirm();
	}
}

void CustomTradeSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	isErrorRspInfo(pRspInfo);
}

void CustomTradeSpi::OnFrontDisconnected(int nReason)
{
	std::cerr << "=====�������ӶϿ�=====" << std::endl;
	std::cerr << "�����룺 " << nReason << std::endl;
}

void CustomTradeSpi::OnHeartBeatWarning(int nTimeLapse)
{
	std::cerr << "=====����������ʱ=====" << std::endl;
	std::cerr << "���ϴ�����ʱ�䣺 " << nTimeLapse << std::endl;
}

void CustomTradeSpi::OnRspUserLogout(
	CThostFtdcUserLogoutField *pUserLogout,
	CThostFtdcRspInfoField *pRspInfo,
	int nRequestID,
	bool bIsLast)
{
	if (!isErrorRspInfo(pRspInfo))
	{
		loginFlag = false; // �ǳ��Ͳ����ٽ����� 
		std::cout << "=====�˻��ǳ��ɹ�=====" << std::endl;
		std::cout << "�����̣� " << pUserLogout->BrokerID << std::endl;
		std::cout << "�ʻ����� " << pUserLogout->UserID << std::endl;
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
		std::cout << "=====Ͷ���߽�����ȷ�ϳɹ�=====" << std::endl
			<< "�˻��� " << gInvesterID << std::endl
			<< "ȷ�����ڣ� " << pSettlementInfoConfirm->ConfirmDate << std::endl
			<< "ȷ��ʱ�䣺 " << pSettlementInfoConfirm->ConfirmTime << std::endl;
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
		std::cout << "=====��ѯ��Լ����ɹ�=====" << std::endl;
		std::cout << "���������룺 " << pInstrument->ExchangeID << std::endl;
		std::cout << "��Լ���룺 " << pInstrument->InstrumentID << std::endl;
		std::cout << "��Լ�ڽ������Ĵ��룺 " << pInstrument->ExchangeInstID << std::endl;
		std::cout << "ִ�мۣ� " << pInstrument->StrikePrice << std::endl;
		std::cout << "�����գ� " << pInstrument->EndDelivDate << std::endl;
		std::cout << "��ǰ����״̬�� " << pInstrument->IsTrading << std::endl;
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
		ARecord.Balance = pTradingAccount->Balance;					//�ڻ�����׼����
		ARecord.Available = pTradingAccount->Available;				//�����ʽ�
		ARecord.CurrMargin = pTradingAccount->CurrMargin;			//��ǰ��֤��
		ARecord.PositionProfit = pTradingAccount->PositionProfit;	//����ӯ��
		ARecord.CloseProfit = pTradingAccount->CloseProfit;			//ƽ��ӯ��
		ARecord.Commission = pTradingAccount->Commission;			//������
		ARecord.AveWin = ARecord.DoneDeal > 0 ? ARecord.CloseProfit / ARecord.DoneDeal : 0;	//ƽ��ÿ��ӯ��
																							//�ϲ�
		if (UpdateFlagAccount == 0)	return;
		WaitForSingleObject(MutexAccount, INFINITE);				//����
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
		ReleaseMutex(MutexAccount);									//����
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
			//if (pInvestorPosition->TodayPosition == 0)	return;		//ֻ����Щ�гֲֵ�
			
			//std::cout << "=====��ѯͶ���ֲֳ߳ɹ�=====" << std::endl;
			//std::cout << "��Լ���룺 " << pInvestorPosition->InstrumentID << std::endl;
			//std::cout << "nRequestID�� " << nRequestID << std::endl;
			//std::cout << "bIsLast�� " << bIsLast << std::endl;
			//std::cout << "���ּ۸� " << pInvestorPosition->UseMargin << std::endl;
			//std::cout << "�������� " << pInvestorPosition->OpenVolume << std::endl;
			//std::cout << "���ַ��� " << pInvestorPosition->PosiDirection << std::endl;    // 1��  2��  3��
			//std::cout << "���ճֲ֣�" << pInvestorPosition->TodayPosition << std::endl;
			//std::cout << "�ֲ֣�" << pInvestorPosition->Position << std::endl;
			//std::cout << "����ӯ����" << pInvestorPosition->PositionProfit << std::endl;
			//std::cout << "ƽ��ӯ����" << pInvestorPosition->CloseProfit << std::endl;
			//std::cout << "�����ѣ�" << pInvestorPosition->Commission << std::endl;
			//std::cout << "ռ�ñ�֤��" << pInvestorPosition->OpenCost * pInvestorPosition->MarginRateByMoney << std::endl;
			bool HaveBeforePosition = pInvestorPosition->TodayPosition == 0 && pInvestorPosition->Position > 0;	//���
			//��Ʒ�����
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
			if (pInvestorPosition->PosiDirection == '2') {//��
				temr.OpenVolume_Buy += pInvestorPosition->OpenVolume;				//ȫ������	
				temr.TodayPosition_Buy += pInvestorPosition->Position;				//ȫ���ֲ�
				//std::cout << "OpenPrice:  " << (pInvestorPosition->Position>0 ? pInvestorPosition->UseMargin / pInvestorPosition->Position : 0 )<< endl;
				temr.OpenPrice_Buy = pInvestorPosition->Position > 0 ? pInvestorPosition->UseMargin / pInvestorPosition->Position : 0;					//���־���
			}
			else if (pInvestorPosition->PosiDirection == '3') {//��
				temr.OpenVolume_Sell += pInvestorPosition->OpenVolume;
				temr.TodayPosition_Sell += pInvestorPosition->Position;
				//std::cout << "OpenPrice:  " << (pInvestorPosition->Position>0 ? pInvestorPosition->UseMargin / pInvestorPosition->Position : 0) << endl;
				temr.OpenPrice_Sell = pInvestorPosition->Position>0 ? pInvestorPosition->UseMargin / pInvestorPosition->Position : 0;		//���־���
			}
			temr.CloseProfit += pInvestorPosition->CloseProfit;						//ƽ��ӯ��
			temr.PositionProfit += pInvestorPosition->PositionProfit;				//����ӯ��
			temr.Commission += pInvestorPosition->Commission;						//������
			temr.Margin += pInvestorPosition->OpenCost * pInvestorPosition->MarginRateByMoney;
			//����ƽ�ֻ��ߵ�Ʒ��ƽ��
			
			if (( MdClear[s] > 0 && pInvestorPosition->Position > 0)) {
				string tems = pInvestorPosition->InstrumentID;
				it = MdHigh.find(tems);
				if (it == MdHigh.end())
					std::cout << ">>>>û�ж��ĵ�ǰƷ�ֵ����ݣ��޷�ƽ��<<<<" << endl;
				else {
					std::cout << "�������: " << pInvestorPosition->InstrumentID << "  " << pInvestorPosition->PosiDirection << endl;
					TThostFtdcPriceType price;
					if (pInvestorPosition->PosiDirection == '2') { //����ƽ��
						price = MdLow[tems];
						if (!HaveBeforePosition) {
							if (MdClear[s] < 2) {
								std::cout << "�Ѿ�ƽ���ˣ�" << std::endl;
								return;
							}
							MdClear[s] -= 2;
						}
						std::cout << "�۸�" << price << endl;
						if (pInvestorPosition->TodayPosition >0)
							reqOrderInsert(pInvestorPosition->InstrumentID, price, pInvestorPosition->TodayPosition, THOST_FTDC_D_Sell, THOST_FTDC_OF_CloseToday, THOST_FTDC_OPT_LimitPrice);
						if (pInvestorPosition->Position - pInvestorPosition->TodayPosition > 0)
							reqOrderInsert(pInvestorPosition->InstrumentID, price, pInvestorPosition->Position - pInvestorPosition->TodayPosition, THOST_FTDC_D_Sell, THOST_FTDC_OF_Close, THOST_FTDC_OPT_LimitPrice);
					}
					else if (pInvestorPosition->PosiDirection == '3') { //����ƽ��
						price = MdHigh[tems];
						if (!HaveBeforePosition) {
							if (MdClear[s] != 1 && MdClear[s] != 3) {
								std::cout << "�Ѿ�ƽ���ˣ�" << std::endl;
								return;
							}
							MdClear[s] -= 1;
						}
						std::cout << "�۸�" << price << endl;
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
		std::cout << "=====����¼��ɹ�=====" << std::endl;
		std::cout << "��Լ���룺 " << pInputOrder->InstrumentID << std::endl;
		std::cout << "�۸� " << pInputOrder->LimitPrice << std::endl;
		std::cout << "������ " << pInputOrder->VolumeTotalOriginal << std::endl;
		std::cout << "���ַ��� " << pInputOrder->Direction << std::endl;
		
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
		/*std::cout << "=====���������ɹ�=====" << std::endl;
		std::cout << "��Լ���룺 " << pInputOrderAction->InstrumentID << std::endl;
		std::cout << "������־�� " << pInputOrderAction->ActionFlag;*/
	}
}
/*
ͨ���������ı���ָ�ReqOrderInsert����CTP��̨����ͻ��˷���OnRtnOrder��Ϣ������OrderSubmitStatusΪ���Ѿ��ύ����
OrderStatusΪ��δ֪����ͬʱCTP��̨���ñ���ָ��ת������Ӧ�Ľ�����ϵͳ��������ϵͳͬ����Ա���������Ӧ�ļ�飬
��۸��Ƿ񳬳��ǵ�ͣ�塢����ָ���Ƿ����õȵȣ�δͨ��������ϵͳ���ı�����
CTP�յ�������ϵͳ��Ӧ��Ҳ����ͻ��˷���OnRtnOrder��Ϣ������OrderSubmitStatusΪ�������Ѿ����ܾ�����OrderStatusΪ����������
*/
void CustomTradeSpi::OnRtnOrder(CThostFtdcOrderField *pOrder)
{
	//	Sleep(1000);
	//sprintf(str, "%d", pOrder->OrderSubmitStatus);
	//����״̬0=�Ѿ��ύ��3=�Ѿ����� 4=���ܾ�

										///ȫ���ɽ� #define THOST_FTDC_OST_AllTraded '0'
										///���ֳɽ����ڶ����� #define THOST_FTDC_OST_PartTradedQueueing '1'
										///���ֳɽ����ڶ����� #define THOST_FTDC_OST_PartTradedNotQueueing '2' 
										///δ�ɽ����ڶ����� #define THOST_FTDC_OST_NoTradeQueueing '3'
										///δ�ɽ����ڶ����� #define THOST_FTDC_OST_NoTradeNotQueueing '4'
										///���� #define THOST_FTDC_OST_Canceled '5'
										///δ֪����ʾThost�Ѿ������û���ί��ָ���û��ת���������� #define THOST_FTDC_OST_Unknown 'a'
										///��δ���� #define THOST_FTDC_OST_NotTouched 'b'
										string SysID = pOrder->OrderSysID;
										std::cout << "=====�յ�����Ӧ��=====" << std::endl;
										std::cout << "�����ύ״̬: " << pOrder->OrderSubmitStatus << std::endl;
										std::cout << "����״̬: " << pOrder->OrderStatus << std::endl;
										std::cout << "�������: " << pOrder->SequenceNo << std::endl;
										std::cout << "��Լ����: " << pOrder->InstrumentID << std::endl;
										std::cout << "�����۸�: " << pOrder->LimitPrice << std::endl;
										std::cout << "��Ͽ�ƽ��־: " << pOrder->CombOffsetFlag[0] << std::endl;
										std::cout << "�������: " << pOrder->OrderSysID << std::endl;
										std::cout << "��ų���: " << SysID.length() << std::endl;
										std::cout << "��: " << pOrder->VolumeTotal << std::endl;
										
	if (pOrder->OrderSubmitStatus == '0' &&  pOrder->OrderStatus != '0' && pOrder->VolumeTotal > 0 && SysID.length() > 0) {
		OrderRecord TemOrder;
		strcpy_s(TemOrder.InstrumentID, pOrder->InstrumentID);
		TemOrder.Price = pOrder->LimitPrice;
		TemOrder.VolumeTotal = pOrder->VolumeTotal;
		TemOrder.Direction = pOrder->Direction;
		TemOrder.OffsetFlag = pOrder->CombOffsetFlag[0];
		strcpy_s(TemOrder.TradeTime, pOrder->InsertTime);
		TemOrder.status = "�����ύ";
		OrderR.push_back(TemOrder);
	}

	if (isMyOrder(pOrder))
	{
		if (isTradingOrder(pOrder))
		{
			/*std::cout << "�˻�: " << gInvesterID << std::endl
			<< "--->>> �ȴ��ɽ��У�" << std::endl;*/
			//reqOrderAction(pOrder); // ������Գ���
			//reqUserLogout(); // �ǳ�����
		}
		else if (pOrder->OrderStatus == THOST_FTDC_OST_Canceled)
		{
			std::cout << "�˻�: " << gInvesterID << std::endl
				<< "--->>> �����ɹ���" << std::endl;
			ARecord.UnDoneNum += pOrder->VolumeTotal; 
			OrderRecord TemOrder;
			strcpy_s(TemOrder.InstrumentID, pOrder->InstrumentID);
			TemOrder.Price = pOrder->LimitPrice;
			TemOrder.VolumeTotal = pOrder->VolumeTotal;
			TemOrder.Direction = pOrder->Direction;
			TemOrder.OffsetFlag = pOrder->CombOffsetFlag[0];
			strcpy_s(TemOrder.TradeTime, pOrder->InsertTime);
			TemOrder.status = "����";
			OrderR.push_back(TemOrder);
		}
	}
}

void CustomTradeSpi::OnRtnTrade(CThostFtdcTradeField *pTrade)
{
	std::cout << "=====�����ɹ��ɽ�=====" << std::endl;
	std::cout << "�ɽ�ʱ�䣺 " << pTrade->TradeTime << std::endl;
	std::cout << "��Լ���룺 " << pTrade->InstrumentID << std::endl;
	std::cout << "�ɽ��۸� " << pTrade->Price << std::endl;
	std::cout << "�ɽ����� " << pTrade->Volume << std::endl;
	std::cout << "��ƽ�ַ��� " << pTrade->Direction << std::endl;
	std::cout << "��ƽ��־�� " << pTrade->OffsetFlag << std::endl;
	OrderRecord TemOrder;
	strcpy_s(TemOrder.InstrumentID, pTrade->InstrumentID);
	TemOrder.Price = pTrade->Price;
	TemOrder.VolumeTotal = pTrade->Volume;
	TemOrder.Direction = pTrade->Direction;
	TemOrder.OffsetFlag = pTrade->OffsetFlag;
	strcpy_s(TemOrder.TradeTime, pTrade->TradeTime);
	TemOrder.status = "�ɽ�";
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
		//		std::cout << "test�� " << vpos << std::endl;
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

	//�ϲ�
	{
		WaitForSingleObject(MutexStrategy, INFINITE);				//����
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
		ReleaseMutex(MutexStrategy);									//����
	}


	ARecord.DoneNum += pTrade->Volume;
	if (pTrade->OffsetFlag != THOST_FTDC_OF_Open)	//ֻ��¼ƽ�֣��ѳɹ�ƽ�ֵ���һ�γɹ��Ĳ���
		ARecord.DoneDeal += pTrade->Volume;
}

bool CustomTradeSpi::isErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{
	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);
	if (bResult)
		std::cerr << "���ش���--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << std::endl;
	return bResult;
}

void CustomTradeSpi::reqUserLogin()
{
	CThostFtdcReqUserLoginField loginReq;
	memset(&loginReq, 0, sizeof(loginReq));
	strcpy_s(loginReq.BrokerID, gBrokerID);
	strcpy_s(loginReq.UserID, gInvesterID);
	strcpy_s(loginReq.Password, gInvesterPassword);
	static int requestID = 0; // ������
	int rt = g_pTradeUserApi->ReqUserLogin(&loginReq, requestID);
	std::cout << "�˻�: " << gInvesterID << std::endl;
	if (!rt)
		std::cout << ">>>>>>���͵�¼����ɹ�" << std::endl;
	else
		std::cerr << "--->>>���͵�¼����ʧ��" << std::endl;
}

void CustomTradeSpi::reqUserLogout()
{
	CThostFtdcUserLogoutField logoutReq;
	memset(&logoutReq, 0, sizeof(logoutReq));
	strcpy_s(logoutReq.BrokerID, gBrokerID);
	strcpy_s(logoutReq.UserID, gInvesterID);
	static int requestID = 0; // ������
	int rt = g_pTradeUserApi->ReqUserLogout(&logoutReq, requestID);
	if (!rt)
		std::cout << ">>>>>>���͵ǳ�����ɹ�" << std::endl;
	else
		std::cerr << "--->>>���͵ǳ�����ʧ��" << std::endl;
}


void CustomTradeSpi::reqSettlementInfoConfirm()
{
	CThostFtdcSettlementInfoConfirmField settlementConfirmReq;
	memset(&settlementConfirmReq, 0, sizeof(settlementConfirmReq));
	strcpy_s(settlementConfirmReq.BrokerID, gBrokerID);
	strcpy_s(settlementConfirmReq.InvestorID, gInvesterID);
	static int requestID = 0; // ������
	int rt = g_pTradeUserApi->ReqSettlementInfoConfirm(&settlementConfirmReq, requestID);
	if (!rt)
	{
		std::cout << ">>>>>>����Ͷ���߽�����ȷ������ɹ�" << std::endl;
	}
	else
	{
		std::cerr << "--->>>����Ͷ���߽�����ȷ������ʧ��" << std::endl;
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
//�ʽ�����
void CustomTradeSpi::GetMoneyLine() {
	static int requestID = 0;

	CThostFtdcQrySettlementInfoField pQrySettlementInfo;
	strcpy_s(pQrySettlementInfo.BrokerID, gBrokerID);
	strcpy_s(pQrySettlementInfo.InvestorID, gInvesterID);
	NowDay.copy(pQrySettlementInfo.TradingDay, NowDay.length(), 0);
	pQrySettlementInfo.TradingDay[8] = 0;
	int result = g_pTradeUserApi->ReqQrySettlementInfo(&pQrySettlementInfo, requestID++);
	if (!result)
		std::cout << ">>>>>>��ѯ���㵥�ɹ�  " << pQrySettlementInfo.TradingDay <<  std::endl;
	else
	{
		std::cout << ">>>>>>��ѯ���㵥ʧ��  " << pQrySettlementInfo.TradingDay <<  std::endl;
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
///Ͷ���߽�������Ӧ
void CustomTradeSpi::OnRspQrySettlementInfo(CThostFtdcSettlementInfoField *pSettlementInfo, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	if (!isErrorRspInfo(pRspInfo)) {
		if (pSettlementInfo == NULL) {
			std::cout << "Ͷ���߽�������Ӧ�ǿ�ָ�밡" << std::endl;
			return;
		}
		string FindResult = StringPair("Balance c/f��", pSettlementInfo->Content);
		if (FindResult.length() > 0)
		{
			std::cout << "���ҽ��:" << NowDay << "     " << FindResult << std::endl;
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
	static int requestID = 0; // ������
	VariRecord.clear();
	int rt = g_pTradeUserApi->ReqQryInstrument(&instrumentReq, requestID);
	/*if (!rt)
	std::cout << ">>>>>>���ͺ�Լ��ѯ����ɹ�" << std::endl;
	else
	std::cerr << "--->>>���ͺ�Լ��ѯ����ʧ��" << std::endl;*/
}

void CustomTradeSpi::reqQueryTradingAccount()
{
	if (loginFlag == false)	return;
	CThostFtdcQryTradingAccountField tradingAccountReq;
	memset(&tradingAccountReq, 0, sizeof(tradingAccountReq));
	strcpy_s(tradingAccountReq.BrokerID, gBrokerID);
	strcpy_s(tradingAccountReq.InvestorID, gInvesterID);
	static int requestID = 0; // ������
	//std::this_thread::sleep_for(std::chrono::milliseconds(500)); // ��ʱ����Ҫͣ��һ����ܲ�ѯ�ɹ�
	Sleep(200);
	int rt = g_pTradeUserApi->ReqQryTradingAccount(&tradingAccountReq, requestID);
	if (!rt) {
		std::cout << ">>>>>>����Ͷ�����ʽ��˻���ѯ����ɹ�" << std::endl;
	}
	else
	{
		std::cerr << "--->>>����Ͷ�����ʽ��˻���ѯ����ʧ��,���²�ѯ" << std::endl;
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
	static int requestID = -1001; // �����ţ�������ʶ�������������߷��ģ����������һ�ߵ�ctp��������
	int rt = g_pTradeUserApi->ReqQryInvestorPosition(&postionReq, requestID);
	if (!rt) {
		std::cout << ">>>>>>����Ͷ���ֲֲ߳�ѯ����ɹ�" << std::endl;
		//UpdateFlagVariable = 1;
	}
	if (rt)
	{
		//����Ҫ�����������ͺü��Σ���Ҳ��֪��Ϊʲô
		std::cerr << "--->>>����Ͷ���ֲֲ߳�ѯ����ʧ��,���·�������  " << rt << std::endl;
		Sleep(500);
		if (time < 10)	reqQueryInvestorPosition(g_pTradeInstrumentID, time++);
	}
	//reqMoneyLine();
}

void CustomTradeSpi::reqOrderInsert()
{
	
	CThostFtdcInputOrderField orderInsertReq;
	memset(&orderInsertReq, 0, sizeof(orderInsertReq));
	///���͹�˾����
	strcpy_s(orderInsertReq.BrokerID, gBrokerID);
	///Ͷ���ߴ���
	strcpy_s(orderInsertReq.InvestorID, gInvesterID);
	//�û�����
	strcpy_s(orderInsertReq.UserID, gInvesterID);
	///��Լ����
	strcpy_s(orderInsertReq.InstrumentID, g_pTradeInstrumentID);
	///��������
	strcpy_s(orderInsertReq.OrderRef, order_ref);

	///�����۸�����: �޼�
	orderInsertReq.OrderPriceType = THOST_FTDC_OPT_LimitPrice; //ֻ����limit
															   //��Ч����
	orderInsertReq.TimeCondition = THOST_FTDC_TC_GTC;
	///��������: 
	orderInsertReq.Direction = gTradeDirection;
	///��Ͽ�ƽ��־: ����
	orderInsertReq.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
	///���Ͷ���ױ���־
	orderInsertReq.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
	///�۸�
	orderInsertReq.LimitPrice = gLimitPrice;
	///������1
	orderInsertReq.VolumeTotalOriginal = 1;
	///��Ч������: ������Ч
	orderInsertReq.TimeCondition = THOST_FTDC_TC_GFD;
	///�ɽ�������: �κ�����
	orderInsertReq.VolumeCondition = THOST_FTDC_VC_AV;
	///��С�ɽ���: 1
	orderInsertReq.MinVolume = 0;
	///��������: ����
	orderInsertReq.ContingentCondition = THOST_FTDC_CC_Immediately;
	///ǿƽԭ��: ��ǿƽ
	orderInsertReq.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
	///�Զ������־: ��
	orderInsertReq.IsAutoSuspend = 0;
	///�û�ǿ����־: ��
	orderInsertReq.UserForceClose = 0;



	static int requestID = 0; // ������
	int rt = g_pTradeUserApi->ReqOrderInsert(&orderInsertReq, ++requestID);
	if (!rt)
		std::cout << ">>>>>>���ͱ���¼������ɹ�" << std::endl;
	else
	{
		std::cerr << "--->>>���ͱ���¼������ʧ��,���·���" << std::endl;

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
	///���͹�˾����
	strcpy_s(orderInsertReq.BrokerID, gBrokerID);
	///Ͷ���ߴ���
	strcpy_s(orderInsertReq.InvestorID, gInvesterID);
	///��Լ����
	strcpy_s(orderInsertReq.InstrumentID, instrumentID);
	///��������
	strcpy_s(orderInsertReq.OrderRef, order_ref);
	///�����۸�����: �޼�
	orderInsertReq.OrderPriceType = PTypeType;	//THOST_FTDC_OPT_AskPrice1
												///��������: 
	orderInsertReq.Direction = direction;
	///��Ͽ�ƽ��־: ����
	orderInsertReq.CombOffsetFlag[0] = type; //THOST_FTDC_OF_CloseToday;
											 ///���Ͷ���ױ���־
	orderInsertReq.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
	///�۸�
	orderInsertReq.LimitPrice = price;
	///������1
	orderInsertReq.VolumeTotalOriginal = volume;
	///��Ч������: ������Ч
	orderInsertReq.TimeCondition = THOST_FTDC_TC_GFD;
	///�ɽ�������: �κ�����
	orderInsertReq.VolumeCondition = THOST_FTDC_VC_AV;
	///��С�ɽ���: 1
	orderInsertReq.MinVolume = 1;
	///��������: ����
	orderInsertReq.ContingentCondition = THOST_FTDC_CC_Immediately;
	///ǿƽԭ��: ��ǿƽ
	orderInsertReq.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
	///�Զ������־: ��
	orderInsertReq.IsAutoSuspend = 0;
	///�û�ǿƽ��־: ��
	orderInsertReq.UserForceClose = 0;

	static int requestID = 0; // ������
	int rt = g_pTradeUserApi->ReqOrderInsert(&orderInsertReq, ++requestID);
	if (!rt)
		std::cout << ">>>>>>���ͱ���¼������ɹ�" << std::endl;
	else
		std::cerr << "--->>>���ͱ���¼������ʧ��" << std::endl;
}

void CustomTradeSpi::reqOrderAction(CThostFtdcOrderField *pOrder)
{
	static bool orderActionSentFlag = false; // �Ƿ����˱���
	if (orderActionSentFlag)
		return;

	CThostFtdcInputOrderActionField orderActionReq;
	memset(&orderActionReq, 0, sizeof(orderActionReq));
	///���͹�˾����
	strcpy_s(orderActionReq.BrokerID, pOrder->BrokerID);
	///Ͷ���ߴ���
	strcpy_s(orderActionReq.InvestorID, pOrder->InvestorID);
	///������������
	//	TThostFtdcOrderActionRefType	OrderActionRef;
	///��������
	strcpy_s(orderActionReq.OrderRef, pOrder->OrderRef);
	///������
	//	TThostFtdcRequestIDType	RequestID;
	///ǰ�ñ��
	orderActionReq.FrontID = trade_front_id;
	///�Ự���
	orderActionReq.SessionID = session_id;
	///����������
	//	TThostFtdcExchangeIDType	ExchangeID;
	///�������
	//	TThostFtdcOrderSysIDType	OrderSysID;
	///������־
	orderActionReq.ActionFlag = THOST_FTDC_AF_Delete;
	///�۸�
	//	TThostFtdcPriceType	LimitPrice;
	///�����仯
	//	TThostFtdcVolumeType	VolumeChange;
	///�û�����
	//	TThostFtdcUserIDType	UserID;
	///��Լ����
	strcpy_s(orderActionReq.InstrumentID, pOrder->InstrumentID);
	static int requestID = 0; // ������
	int rt = g_pTradeUserApi->ReqOrderAction(&orderActionReq, ++requestID);
	if (!rt)
		std::cout << ">>>>>>���ͱ�����������ɹ�������" << std::endl;
	else
		std::cerr << "--->>>���ͱ�����������ʧ�ܣ�����" << std::endl;
	orderActionSentFlag = true;
}

bool CustomTradeSpi::isMyOrder(CThostFtdcOrderField *pOrder)
{
	//������ʷ����ʱ����ʱ����Ϊ���Լ��ı���
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
	if (day[7] > '9') {	//��λ��  ��
		day[7] = '0';
		day[6] += 1;
	}
	if (day[6] >= '3' && day[7] > '2') {	//ʮλ��  ��
		day[5] += 1;
		day[6] = '0';
		day[7] = '1';
	}
	if (day[4] == '0'  && day[5] > '9') {	//��λ��  ��
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
	//std::cout << std::endl << "=====Ͷ�����ʽ��˻�=====" << std::endl
	//	<< "Ͷ�����˺ţ� " << gInvesterID << std::endl
	//	<< "�����ʽ� " << ARecord.Available << std::endl
	//	<< "�ڻ�����׼���� " << ARecord.Balance << std::endl
	//	<< "��ǰ��֤��: " << ARecord.CurrMargin << std::endl
	//	<< "����ӯ���� " << ARecord.PositionProfit << std::endl
	//	<< "ƽ��ӯ���� " << ARecord.CloseProfit << std::endl
	//	<< "�����ѣ� " << ARecord.Commission << std::endl
	//	<< "�ɹ��ɽ�����(ƽ��)�� " << ARecord.DoneNum << std::endl		//���ƽ�ֲ�������Ϊ�ǳɹ��ɹ�
	//	<< "�޷��ɽ�������(����)�� " << ARecord.UnDoneNum << std::endl
	//	<< "ƽ��ÿ��ƽ��ӯ���� " << ARecord.AveWin << std::endl;

	std::cout << std::endl << "=====����Ʒ���������=====" << std::endl;
	int len = VariRecord.size();
	for (int i = 0; i < len; i++) {
		VariableRecord temr = VariRecord[i];
		std::cout << "��ԼID�� " << temr.InstrumentID << std::endl
			<< "���տ����������� " << temr.OpenVolume_Buy << std::endl
			<< "���տ����������� " << temr.OpenVolume_Sell << std::endl
			<< "��ǰ��ͷ�ֲ֣� " << temr.TodayPosition_Buy << "   �ֲ־��ۣ� " << temr.OpenPrice_Buy << std::endl
			<< "��ǰ��ͷ�ֲ֣� " << temr.TodayPosition_Sell << "   �ֲ־��ۣ� " << temr.OpenPrice_Sell << std::endl
			<< "�����ѣ� " << temr.Commission << std::endl
			<< "����ӯ���� " << temr.PositionProfit << std::endl
			<< "ƽ��ӯ���� " << temr.CloseProfit << std::endl
			<< "��֤�� " << temr.Margin << std::endl
			<< "====================" << std::endl;
	}
	/*std::cout << std::endl << "=====���������������=====" << StraRecord.size() << std::endl;
	for (int i = 0; i < StraRecord.size(); i++) {
		SteategyRecord temsr = StraRecord[i];
		std::cout << "Strategy Number: " << temsr.StrategyNum << endl;
		for (int j = 0; j < temsr.StrategyVariable.size(); j++) {
			int TradeVolume = (temsr.StrategyVariable[j].OpenVolume_Buy + temsr.StrategyVariable[j].OpenVolume_Sell);
			std::cout << "Ʒ��:  " << temsr.StrategyVariable[j].InstrumentID << endl
				<< "ƽ��ÿ��ӯ��:  " << (TradeVolume > 0 ? temsr.StrategyVariable[j].CloseProfit / TradeVolume : 0) << endl
				<< "��������:  " << temsr.StrategyVariable[j].OpenVolume_Buy + temsr.StrategyVariable[j].OpenVolume_Sell << endl;
		}
	}*/
}

void CustomTradeSpi::SetPara(string broker, string invester, string password, CThostFtdcTraderApi* api, string addr, string UserProductInfo, string AuthCode) {
	int i = 0;
	for (i = 0; i < broker.length(); i++) gBrokerID[i] = broker[i];  gBrokerID[i] = 0;
	for (i = 0; i < invester.length(); i++) gInvesterID[i] = invester[i];  gInvesterID[i] = 0;
	for (i = 0; i < password.length(); i++) gInvesterPassword[i] = password[i];  gInvesterPassword[i] = 0;
	for (i = 0; i < addr.length(); i++) gTradeFrontAddr[i] = addr[i];  gTradeFrontAddr[i] = 0;
	//��½��֤
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