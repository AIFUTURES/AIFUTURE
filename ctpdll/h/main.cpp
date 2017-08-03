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
using namespace std;

#pragma comment (lib, "thostmduserapi.lib")  
#pragma comment (lib, "thosttraderapi.lib") 


// ---- ȫ�ֱ��� ---- //  

//simow
TThostFtdcBrokerIDType gBrokerID = "9999";                         // ģ�⾭���̴���  
TThostFtdcInvestorIDType gInvesterID = "083370";                         // Ͷ�����˻���  
TThostFtdcPasswordType gInvesterPassword = "013579";                     // Ͷ��������  

// �������  
CThostFtdcMdApi *g_pMdUserApi = nullptr;                           // ����ָ��  
char gMdFrontAddr[] = "tcp://180.168.146.187:10010";               // ģ������ǰ�õ�ַ
//char gMdFrontAddr[] = "tcp://180.168.146.187:10031";               // �ǿ���ʱ��
// ���ײ���  
CThostFtdcTraderApi *g_pTradeUserApi = nullptr;                    // ����ָ��  
char gTradeFrontAddr[] = "tcp://180.168.146.187:10000 ";            // ģ�⽻��ǰ�õ�ַ  
//char gTradeFrontAddr[] = "tcp://180.168.146.187:10030";            // �ǿ���ʱ��
TThostFtdcInstrumentIDType g_pTradeInstrumentID = "Cu1710";         // �����׵ĺ�Լ����  
TThostFtdcDirectionType gTradeDirection = THOST_FTDC_D_Buy;       // ��������  
TThostFtdcPriceType gLimitPrice = 3400;                            // ���׼۸�

char *g_pInstrumentID[] = { "rb1710" };                     // �����Լ�����б�
int instrumentNum = 1;                                             // �����Լ�������� 

string getTime()
{
	time_t timep;
	time(&timep);
	char tmp[64];
	strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", localtime(&timep));
	return tmp;
}

int main()
{
	// �˺�����  


	Monitor a;
	a.Prepare();
	a.Connect();
	while (1) {
		a.UpdateRelateAccount();
		cout << "===========" << getTime() << "===========" << endl;
		a.Check();
	}
	
	cout << "������" << endl;
	getchar();
	return 0;
}