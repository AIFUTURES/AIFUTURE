#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <string.h>
#include <map>
#include "CustomMdSpi.h"
//#include "TickToKlineHelper.h"
using namespace std;
// ---- ȫ�ֲ������� ---- //
extern CThostFtdcMdApi *g_pMdUserApi;            // ����ָ��
extern char gMdFrontAddr[];                      // ģ������ǰ�õ�ַ
extern TThostFtdcBrokerIDType gBrokerID;         // ģ�⾭���̴���
extern TThostFtdcInvestorIDType gInvesterID;     // Ͷ�����˻���
extern TThostFtdcPasswordType gInvesterPassword; // Ͷ��������
extern char *g_pInstrumentID[];                  // �����Լ�����б����С��ϡ���֣��������ѡһ��
extern int instrumentNum;                        // �����Լ��������

extern int ConnectFlag;


extern map < string, TThostFtdcPriceType > MdHigh;
extern map < string, TThostFtdcPriceType > MdLow;
// ---- ctp_api�ص����� ---- //
// ���ӳɹ�Ӧ��
void CustomMdSpi::OnFrontConnected()
{
	std::cout << "=====���������������ӳɹ�=====" << std::endl;
	// ��ʼ��¼
	CThostFtdcReqUserLoginField loginReq;
	memset(&loginReq, 0, sizeof(loginReq));
	strcpy_s(loginReq.BrokerID, gBrokerID);
	strcpy_s(loginReq.UserID, gInvesterID);
	strcpy_s(loginReq.Password, gInvesterPassword);
	static int requestID = 0; // ������
	int rt = g_pMdUserApi->ReqUserLogin(&loginReq, requestID);
	if (!rt)
	{
		std::cout << ">>>>>>���������¼����ɹ�" << std::endl;
		
	}
	else
		std::cerr << "--->>>���������¼����ʧ��" << std::endl;
}

// �Ͽ�����֪ͨ
void CustomMdSpi::OnFrontDisconnected(int nReason)
{
	std::cerr << "=====�����������ӶϿ�=====" << std::endl;
	std::cerr << "�����룺 " << nReason << std::endl;
}

// ������ʱ����
void CustomMdSpi::OnHeartBeatWarning(int nTimeLapse)
{
	std::cerr << "=====��������������ʱ=====" << std::endl;
	std::cerr << "���ϴ�����ʱ�䣺 " << nTimeLapse << std::endl;
}

// ��¼Ӧ��
void CustomMdSpi::OnRspUserLogin(
	CThostFtdcRspUserLoginField *pRspUserLogin,
	CThostFtdcRspInfoField *pRspInfo,
	int nRequestID,
	bool bIsLast)
{
	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);
	if (!bResult)
	{
		std::cout << "=====�����˻���¼�ɹ�=====" << std::endl;
		std::cout << "�����գ� " << pRspUserLogin->TradingDay << std::endl;
		std::cout << "��¼ʱ�䣺 " << pRspUserLogin->LoginTime << std::endl;
		std::cout << "�����̣� " << pRspUserLogin->BrokerID << std::endl;
		std::cout << "�ʻ����� " << pRspUserLogin->UserID << std::endl;
		// ��ʼ��������
		int rt = g_pMdUserApi->SubscribeMarketData(g_pInstrumentID, instrumentNum);
		for (int i = 0; i < instrumentNum; i++) {
			std::string tems = g_pInstrumentID[i];
			MdHigh.insert(make_pair(tems, 0.0));
			MdLow.insert(make_pair(tems, 0.0));
		}
		if (!rt)
		{
			std::cout << ">>>>>>���Ͷ�����������ɹ�" << std::endl;
			ConnectFlag = 1;
		}
		else
			std::cerr << "--->>>���Ͷ�����������ʧ��" << std::endl;
	}
	else
	{
		std::cerr << "���鷵�ش���--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << std::endl;
		ConnectFlag = 0;
	}
}

void CustomMdSpi::registerMarketData(char **p) {
	
	string s = p[0];
	std::map<string, TThostFtdcPriceType>::iterator it;
	it = MdHigh.find(s);
	std::cout << p[0] << endl;
	if (it == MdHigh.end()) {
		int rt = g_pMdUserApi->SubscribeMarketData(p, 1);
		MdHigh.insert(make_pair(s, 0.0));
		MdLow.insert(make_pair(s, 0.0));
		if (!rt)
			std::cout << ">>>>>>���Ͷ�����������ɹ�" << std::endl;
		else
			std::cerr << "--->>>���Ͷ�����������ʧ��" << std::endl;
	}
	
}

// �ǳ�Ӧ��
void CustomMdSpi::OnRspUserLogout(
	CThostFtdcUserLogoutField *pUserLogout,
	CThostFtdcRspInfoField *pRspInfo,
	int nRequestID,
	bool bIsLast)
{
	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);
	if (!bResult)
	{
		std::cout << "=====�˻��ǳ��ɹ�=====" << std::endl;
		std::cout << "�����̣� " << pUserLogout->BrokerID << std::endl;
		std::cout << "�ʻ����� " << pUserLogout->UserID << std::endl;
	}
	else
	{
		std::cerr << "���鷵�ش���--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << std::endl;
		ConnectFlag = 0;
	}
}

// ����֪ͨ
void CustomMdSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);
	if (bResult) {
		ConnectFlag = 0;
		std::cerr << "���鷵�ش���--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << std::endl;
	}
		
}

// ��������Ӧ��
void CustomMdSpi::OnRspSubMarketData(
	CThostFtdcSpecificInstrumentField *pSpecificInstrument,
	CThostFtdcRspInfoField *pRspInfo,
	int nRequestID,
	bool bIsLast)
{
	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);
	if (!bResult)
	{
		std::cout << "=====��������ɹ�=====" << std::endl;
		std::cout << "��Լ���룺 " << pSpecificInstrument->InstrumentID << std::endl;
		ConnectFlag = 1;
		// �����Ҫ�����ļ��������ݿ⣬�����ﴴ����ͷ,��ͬ�ĺ�Լ�����洢
		/*
		char filePath[100] = { '\0' };
		sprintf(filePath, "C:\\Users\\ZhengJX\\Desktop\\data\\%s_market_data.csv", pSpecificInstrument->InstrumentID);
		std::ofstream outFile;
		outFile.open(filePath, std::ios::out); // �¿��ļ�
		outFile << "��Լ����" << ","
		<< "����ʱ��" << ","
		<< "���¼�" << ","
		<< "�ɽ���" << ","
		<< "���һ" << ","
		<< "����һ" << ","
		<< "����һ" << ","
		<< "����һ" << ","
		<< "�ֲ���" << ","
		<< "������"
		<< std::endl;
		outFile.close();*/
	}
	else
		std::cerr << "���ش���--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << std::endl;
}

// ȡ����������Ӧ��
void CustomMdSpi::OnRspUnSubMarketData(
	CThostFtdcSpecificInstrumentField *pSpecificInstrument,
	CThostFtdcRspInfoField *pRspInfo,
	int nRequestID,
	bool bIsLast)
{
	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);
	if (!bResult)
	{
		std::cout << "=====ȡ����������ɹ�=====" << std::endl;
		std::cout << "��Լ���룺 " << pSpecificInstrument->InstrumentID << std::endl;
	}
	else
		std::cerr << "���ش���--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << std::endl;
}

// ����ѯ��Ӧ��
void CustomMdSpi::OnRspSubForQuoteRsp(
	CThostFtdcSpecificInstrumentField *pSpecificInstrument,
	CThostFtdcRspInfoField *pRspInfo,
	int nRequestID,
	bool bIsLast)
{
	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);
	if (!bResult)
	{
		std::cout << "=====����ѯ�۳ɹ�=====" << std::endl;
		std::cout << "��Լ���룺 " << pSpecificInstrument->InstrumentID << std::endl;
	}
	else
		std::cerr << "���ش���--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << std::endl;
}

// ȡ������ѯ��Ӧ��
void CustomMdSpi::OnRspUnSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);
	if (!bResult)
	{
		std::cout << "=====ȡ������ѯ�۳ɹ�=====" << std::endl;
		std::cout << "��Լ���룺 " << pSpecificInstrument->InstrumentID << std::endl;
	}
	else
		std::cerr << "���ش���--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << std::endl;
}

// ��������֪ͨ
void CustomMdSpi::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
{
	// ��ӡ���飬�ֶν϶࣬��ȡ����
	std::string tems = pDepthMarketData->InstrumentID;
	MdHigh[tems] = pDepthMarketData->UpperLimitPrice;
	MdLow[tems] = pDepthMarketData->LowerLimitPrice;
	//std::cout << "=====����������=====" << std::endl;
	//std::cout << "�����գ� " << pDepthMarketData->TradingDay << std::endl;
	//std::cout << "���������룺 " << pDepthMarketData->ExchangeID << std::endl;
	//std::cout << "��Լ���룺 " << pDepthMarketData->InstrumentID << std::endl;
	//std::cout << "��Լ�ڽ������Ĵ��룺 " << pDepthMarketData->ExchangeInstID << std::endl;
	//std::cout << "���¼ۣ� " << pDepthMarketData->LastPrice << std::endl;
	//std::cout << "������ " << pDepthMarketData->Volume << std::endl;
	//// ���ֻ��ȡĳһ����Լ���飬������tick�ش����ļ������ݿ�
	//std::cout << pDepthMarketData->InstrumentID << ","
	//	<< pDepthMarketData->UpdateTime << "." << pDepthMarketData->UpdateMillisec << ","
	//	<< pDepthMarketData->LastPrice << ","
	//	<< pDepthMarketData->Volume << ","
	//	<< pDepthMarketData->BidPrice1 << ","
	//	<< pDepthMarketData->BidVolume1 << ","
	//	<< pDepthMarketData->AskPrice1 << ","
	//	<< pDepthMarketData->AskVolume1 << ","
	//	<< pDepthMarketData->OpenInterest << ","
	//	<< pDepthMarketData->Turnover << std::endl;

	// ȡ����������
	int rt = g_pMdUserApi->UnSubscribeMarketData(g_pInstrumentID, instrumentNum);
	if (!rt)
		std::cout << ">>>>>>����ȡ��������������ɹ�" << std::endl;
	else
		std::cerr << "--->>>����ȡ��������������ʧ��" << std::endl;
}

// ѯ������֪ͨ
void CustomMdSpi::OnRtnForQuoteRsp(CThostFtdcForQuoteRspField *pForQuoteRsp)
{
	// ����ѯ�۽��
	std::cout << "=====���ѯ�۽��=====" << std::endl;
	std::cout << "�����գ� " << pForQuoteRsp->TradingDay << std::endl;
	std::cout << "���������룺 " << pForQuoteRsp->ExchangeID << std::endl;
	std::cout << "��Լ���룺 " << pForQuoteRsp->InstrumentID << std::endl;
	std::cout << "ѯ�۱�ţ� " << pForQuoteRsp->ForQuoteSysID << std::endl;
}