#include "clrClass.h"
//extern int a;
clrClass::clrClass(void)
{
	
}

clrClass::~clrClass(void)
{
}

void clrClass::exit() {
	//exit();
	return;
}

void clrClass::Connect(int i) {
	monitor->Connect(i);
}

void clrClass::Check() {
	monitor->Check();
}

void clrClass::EClear() {
	monitor->Clear();
}

void clrClass::ClearAccount(int id) {
	monitor->pTradeUserSpiVector[id]->EmergentFlag = 1;
	monitor->pTradeUserSpiVector[id]->ClearAll();
}

void clrClass::OpenTest() {
	monitor->OpenTest();
}

void clrClass::CloseAccount(int i) {
	monitor->pTradeUserSpiVector[i]->loginFlag = false;
}

void clrClass::initialize() {
	monitor = new Monitor();
}

void clrClass::prepare() {
	monitor->Prepare();
	//monitor->Connect();
}

void clrClass::update() {
	monitor->UpdateRelateAccount();
}

int clrClass::GetConnectFlag() {
	return ConnectFlag;
}
int clrClass::GetTrade(int i) {
	if (i == -1)	return 0;
	return monitor->pTradeUserSpiVector[i]->loginFlag;
}


double clrClass::GetCommission(int i) {
	return monitor->GetCommission(i);
}

String^  clrClass::AccountName(int i) {

	char* c = monitor->GetId(i);
	String^ result = Marshal::PtrToStringAnsi(static_cast<IntPtr>(c));
	return result;
}


int clrClass::AccountNum() {
	return monitor->AccountNum();
}

double clrClass::GetAveWin(int i) {
	return monitor->GetAveWin(i);
}
int clrClass::GetDoneNum(int i) {
	return monitor->GetDoneNum(i);
}
int clrClass::GetUnDoneNum(int i) {
	return monitor->GetUnDoneNum(i);
}
int clrClass::GetDoneDeal(int i) {
	return monitor->GetDoneDeal(i);
}

double clrClass::GetPositionProfit(int i) {
	return monitor->GetPositionProfit(i);
}
double clrClass::GetCloseProfit(int i) {
	return monitor->GetCloseProfit(i);
}
double clrClass::GetBalance(int i) {
	return monitor->GetBalance(i);
}
double clrClass::GetMargin(int i) {
	return monitor->GetMargin(i);
}

//variable

String^ clrClass::GetVariID(int i, int j) {
	char* c = monitor->GetVariID(i,j);
	String^ result = Marshal::PtrToStringAnsi(static_cast<IntPtr>(c));
	return result;
}
int clrClass::VariNum(int i) {
	return monitor->VariNum(i);
}
double clrClass::GetVariAveWin(int i, int j) {
	return 0.0;
}

double clrClass::GetVariPositionProfit(int i, int j) {
	return monitor->GetVariPositionProfit(i, j);
}
double clrClass::GetVariCloseProfit(int i, int j) {
	return monitor->GetVariCloseProfit(i,j);
}
int clrClass::GetVariPosition(int i, int j) {
	return monitor->GetVariPosition(i, j);
}

double clrClass::GetVariCommission(int i, int j) {
	return monitor->GetVariCommission(i, j);
}
int clrClass::GetVariAllOpen(int i, int j) {
	return monitor->GetVariAllOpen(i, j);
}
int clrClass::ClearVariable(int i, int j) {
	string s = monitor->pTradeUserSpiVector[i]->VariRecord[j].InstrumentID;
	return monitor->pTradeUserSpiVector[i]->ClearVariable(s);
}
//OrderRecord
int clrClass::GetOrderLength(int i) {
	if (i == -1)	return 0;
	else			return monitor->pTradeUserSpiVector[i]->OrderR.size();
}

String^  clrClass::GetOrderVari(int i, int j) {
	if (i == -1)	return "";
	char* c = monitor->pTradeUserSpiVector[i]->OrderR[j].InstrumentID;
	String^ result = Marshal::PtrToStringAnsi(static_cast<IntPtr>(c));
	return result;
}
String^  clrClass::GetOrderTime(int i, int j) {
	if (i == -1)	return "";
	char* c = monitor->pTradeUserSpiVector[i]->OrderR[j].TradeTime;
	String^ result = Marshal::PtrToStringAnsi(static_cast<IntPtr>(c));
	return result;
}
int clrClass::GetOrderVol(int i, int j) {
	if (i == -1)	return 0;
	return monitor->pTradeUserSpiVector[i]->OrderR[j].VolumeTotal;
}
double clrClass::GetOrderPrice(int i, int j) {
	if (i == -1)	return 0.0;
	return monitor->pTradeUserSpiVector[i]->OrderR[j].Price;
}
String^  clrClass::GetOrderType(int i, int j) {
	if (i == -1)	return "";
	TThostFtdcDirectionType type1 = monitor->pTradeUserSpiVector[i]->OrderR[j].Direction;
	TThostFtdcOffsetFlagType type2 = monitor->pTradeUserSpiVector[i]->OrderR[j].OffsetFlag;
	if (type2 == '0') {
		return type1 == '0' ? "开多" : "开空";
	}
	else {
		return type1 == '0' ? "平空" : "平多";
	}
}

String^  clrClass::GetOrderStatu(int i, int j) {
	string r = monitor->pTradeUserSpiVector[i]->OrderR[j].status;
	String^ result = marshal_as<String^>(r);
	return result;
}


//资金线
int clrClass::SetMoneyDate(String^ begin, String^ end, int i) {
	string b = marshal_as<std::string>(begin);
	string e = marshal_as<std::string>(end);

	string s1 = b;
	string s2 = e;
	if (s1.length() != s2.length() || s1.length() != 8 || s1 > s2) {
		return 0;
	}
	for (int i = 0; i < s1.length(); i++) {
		if (s1[i] > '9' || s1[i] < '0')	return 0;
		if (s2[i] > '9' || s2[i] < '0')	return 0;
	}
	monitor->pTradeUserSpiVector[i]->NowDay = b;
	monitor->pTradeUserSpiVector[i]->EndDay = e;
	return 1;
}
int clrClass::GetMoneyLength(int i) {
	return monitor->pTradeUserSpiVector[i]->MoneyLine.size();
}
void clrClass::PrepareMoney(int i) {
	//日期格式输入正确才可以读取数据
	
	monitor->pTradeUserSpiVector[i]->MoneyLine.clear();
	while (monitor->pTradeUserSpiVector[i]->reqMoneyLine() == 1);
}

String^ clrClass::GetMoneyValue(int i, int j) {
	string result = monitor->pTradeUserSpiVector[i]->MoneyLine[j].money;
	return marshal_as<String^>(result);
}
String^ clrClass::GetMoneyDate(int i, int j) {
	string result = monitor->pTradeUserSpiVector[i]->MoneyLine[j].date;
	return marshal_as<String^>(result);
}