#pragma once
#include "stdafx.h"
#include <iostream>
#include <Windows.h>
#include "Monitor.h"

extern vector < VariableRecord > VariRecord_Sum;		//�ڸ���Ʒ�ֵı���
extern AccountRecord ARecord_Sum;						//��¼�˻�����Ϣ
extern vector < SteategyRecord > StraRecord_Sum;		//��¼���Ե��������

// �������  
CThostFtdcMdApi *g_pMdUserApi = nullptr;                           // ����ָ��  
CustomMdSpi *pMdUserSpi;
char gMdFrontAddr[] = "tcp://180.168.146.187:10010";               // ģ������ǰ�õ�ַ  10  31
char *g_pInstrumentID[] = { "rb1710" ,"cu1710" ,"cu1709","i1801","IF1712" };        // �����Լ�����б�
int instrumentNum = 5;                                             // �����Լ�������� 		
TThostFtdcBrokerIDType gBrokerID = "9999";                         // ģ�⾭���̴���  
TThostFtdcInvestorIDType gInvesterID = "083370";                         // Ͷ�����˻���  
TThostFtdcPasswordType gInvesterPassword = "013579";                     // Ͷ��������  


HANDLE MutexAccount;
HANDLE MutexVariable;
HANDLE MutexStrategy;
HANDLE UpdateFlag;
// ���ײ���  
TThostFtdcInstrumentIDType g_pTradeInstrumentID = "cu1710";         // �����׵ĺ�Լ����  
TThostFtdcDirectionType gTradeDirection = THOST_FTDC_D_Buy;         // ��������  
TThostFtdcPriceType gLimitPrice = 3400;                             // ���׼۸�
//�潻��Ʒ�ֵ���ͣ����ͣ�ۣ����ʱ�õ�
map < string, TThostFtdcPriceType > MdHigh;
map < string, TThostFtdcPriceType > MdLow;
extern int ConnectFlag;

void Monitor::Prepare() {
	std::cout << "=====׼����Ϣ��=====" << endl;
	////�˺�1
	//InvesterID.push_back("083370");
	//PassWord.push_back("013579");
	//BrokerID.push_back("9999");
	//TradeAddr.push_back("tcp://180.168.146.187:10000");
	////�˺�2
	//InvesterID.push_back("097128");
	//PassWord.push_back("013579");
	//BrokerID.push_back("9999");
	//TradeAddr.push_back("tcp://180.168.146.187:10000");

	/*int len = InvesterID.size();
	if (PassWord.size() != len || BrokerID.size() != len || TradeAddr.size() != len) {
		std::cout << "׼�������ݲ��ܶ��룬�����Ƿ�����©" << endl;
		return;
	}
	for (int i = 0; i < InvesterID.size(); i++) {
		InvesterIDMap.insert(make_pair(InvesterID[i], i));
	}*/

	cout << "��ʼ������..." << endl;
	ConnectFlag = 0;
	g_pMdUserApi = CThostFtdcMdApi::CreateFtdcMdApi();   // ��������ʵ��  
	pMdUserSpi = new CustomMdSpi;       // ��������ص�ʵ��  
	g_pMdUserApi->RegisterSpi(pMdUserSpi);               // ע���¼���  
	g_pMdUserApi->RegisterFront(gMdFrontAddr);           // ��������ǰ�õ�ַ  
	g_pMdUserApi->Init();                                // ��������  
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
		cout << "��ʼ������..." << endl;
		g_pTradeUserApi = CThostFtdcTraderApi::CreateFtdcTraderApi(); // ��������ʵ��   
		CustomTradeSpi *pTradeSpi = new CustomTradeSpi;               // �������׻ص�ʵ��  
		pTradeSpi->SetPara(BrokerID[i], InvesterID[i], PassWord[i], g_pTradeUserApi, TradeAddr[i], ProductID[i], AuthCode[i]);
		g_pTradeUserApi->RegisterSpi(pTradeSpi);                      // ע���¼���  
		g_pTradeUserApi->SubscribePublicTopic(THOST_TERT_RESTART);    // ���Ĺ�����,ֻ���ܵ�½�������
		g_pTradeUserApi->SubscribePrivateTopic(THOST_TERT_RESTART);   // ����˽����  
		g_pTradeUserApi->RegisterFront(pTradeSpi->gTradeFrontAddr);   // ���ý���ǰ�õ�ַ  
		pTradeUserSpiVector.push_back(pTradeSpi);
		pTradeUserApiVector.push_back(g_pTradeUserApi);
		g_pTradeUserApi->Init();                                      // ��������  

	}
	Sleep(1000);
}
void Monitor::Connect(int i) {
	InvesterIDMap.insert(make_pair(InvesterID[i], i));
	int len = InvesterID.size();
	CThostFtdcTraderApi *g_pTradeUserApi = nullptr;
	cout << "��ʼ������..." << endl;
	g_pTradeUserApi = CThostFtdcTraderApi::CreateFtdcTraderApi(); // ��������ʵ��   
	CustomTradeSpi *pTradeSpi = new CustomTradeSpi;               // �������׻ص�ʵ��  
	pTradeSpi->SetPara(BrokerID[i], InvesterID[i], PassWord[i], g_pTradeUserApi, TradeAddr[i], ProductID[i], AuthCode[i]);
	g_pTradeUserApi->RegisterSpi(pTradeSpi);                      // ע���¼���  
	g_pTradeUserApi->SubscribePublicTopic(THOST_TERT_RESTART);    // ���Ĺ�����,ֻ���ܵ�½�������
	g_pTradeUserApi->SubscribePrivateTopic(THOST_TERT_RESTART);   // ����˽����  
	g_pTradeUserApi->RegisterFront(pTradeSpi->gTradeFrontAddr);   // ���ý���ǰ�õ�ַ  
	pTradeUserSpiVector.push_back(pTradeSpi);
	pTradeUserApiVector.push_back(g_pTradeUserApi);
	g_pTradeUserApi->Init();                                      // ��������  
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
	WaitForSingleObject(MutexVariable, INFINITE);							//����
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
			temr.OpenVolume_Buy += tem.OpenVolume_Buy;									//����������
			temr.OpenVolume_Sell += tem.OpenVolume_Sell;								//����������
			temr.TodayPosition_Buy += tem.TodayPosition_Buy;							//��ǰ��ͷ�ֲ�
			temr.TodayPosition_Sell += tem.TodayPosition_Sell;							//��ǰ��ͷ�ֲ�
			temr.OpenPrice_Buy += tem.OpenPrice_Buy*tem.TodayPosition_Buy;				//��ͷ����
			temr.OpenPrice_Sell += tem.OpenPrice_Sell*tem.TodayPosition_Sell;			//��ͷ����
			temr.CloseProfit += tem.CloseProfit;										//ƽ��ӯ��
			temr.PositionProfit += tem.PositionProfit;									//����ӯ��
			temr.Commission += tem.Commission;											//������
			temr.Margin += tem.Margin;													//��֤��
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
	std::cout << "�����������" << std::endl;
	//��������
	WaitForSingleObject(UpdateFlag, INFINITE);				//����
	VariRecord_Sum.clear();
	AccountRecord newAR; ARecord_Sum = newAR;
	for (int i = 0; i < pTradeUserSpiVector.size(); i++) {
		pTradeUserSpiVector[i]->UpdateRecords();
	}
	Sleep(1000);	
	//�ϲ���֮����һ��ƽ��ӯ�����ֲ־���ʲô��
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
	std::cout << std::endl << "=====�����ʽ��˻�=====" << std::endl
		<< "�����ʽ� " << ARecord_Sum.Available << std::endl
		<< "�ڻ�����׼���� " << ARecord_Sum.Balance << std::endl
		<< "��ǰ��֤��: " << ARecord_Sum.CurrMargin << std::endl
		<< "����ӯ���� " << ARecord_Sum.PositionProfit << std::endl
		<< "ƽ��ӯ���� " << ARecord_Sum.CloseProfit << std::endl
		<< "�����ѣ� " << ARecord_Sum.Commission << std::endl
		<< "�ɹ��ɽ������� " << ARecord_Sum.DoneNum << std::endl
		<< "�ɹ�ƽ�������� " << ARecord_Sum.DoneDeal << std::endl
		<< "����������     " << ARecord_Sum.UnDoneNum << std::endl
		<< "ƽ��ÿ��ƽ��ӯ���� " << ARecord_Sum.AveWin << std::endl;

	std::cout << std::endl << "=====����Ʒ���������=====" << std::endl
		<< "====================" << endl;
	int len = VariRecord_Sum.size();
	for (int i = 0; i < len; i++) {
		VariableRecord temr = VariRecord_Sum[i];
		std::cout << "====================" << std::endl
			<< "��ԼID�� " << temr.InstrumentID << std::endl
			<< "���տ����������� " << temr.OpenVolume_Buy << std::endl
			<< "���տ����������� " << temr.OpenVolume_Sell << std::endl
			<< "��ǰ��ͷ�ֲ֣� " << temr.TodayPosition_Buy << "   �ֲ־��ۣ� " << temr.OpenPrice_Buy << std::endl
			<< "��ǰ��ͷ�ֲ֣� " << temr.TodayPosition_Sell << "   �ֲ־��ۣ� " << temr.OpenPrice_Sell << std::endl
			<< "�����ѣ� " << temr.Commission << std::endl
			<< "ƽ��ӯ���� " << temr.PositionProfit << std::endl
			<< "����ӯ���� " << temr.CloseProfit << std::endl
			<< "��֤�� " << temr.Margin << std::endl
			<< "====================" << std::endl;
	}
	/*std::cout << std::endl << "=====���������������=====" << std::endl;
	for (int i = 0; i < StraRecord_Sum.size(); i++) {
		SteategyRecord temsr = StraRecord_Sum[i];
		std::cout << "Strategy Number: " << temsr.StrategyNum << endl;
		for (int j = 0; j < temsr.StrategyVariable.size(); j++) {
			int TradeVolume = (temsr.StrategyVariable[j].OpenVolume_Buy + temsr.StrategyVariable[j].OpenVolume_Sell);
			std::cout << "Ʒ��:  " << temsr.StrategyVariable[j].InstrumentID << endl
				<< "ƽ��ÿ��ӯ��:  " << (TradeVolume > 0 ? temsr.StrategyVariable[j].CloseProfit / TradeVolume : 0) << endl
				<< "��������:  " << temsr.StrategyVariable[j].OpenVolume_Buy + temsr.StrategyVariable[j].OpenVolume_Sell << endl;
		}
	}*/
}