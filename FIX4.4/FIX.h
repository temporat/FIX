#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <list>
#include <map>
#include <vector>
#include <iterator>
#include <thread>
#include <ctime>
#include <fstream> 

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")

#define NOL "BULLNULL"
#define RLEN 65536
#define ending_comb "\x6\x6\x6Stop" 
#define DLLEXPORT  __declspec(dllexport)
#define DLLEXPORTNOT
using namespace std;

struct DLLEXPORT Instrument
{
	string Symbol = NOL;
	string Product = NOL;
	string CFICode = NOL;
	string SecurityType = NOL;
};
struct DLLEXPORT YieldData
{
	string Yield = NOL;
	string NoPartyID = NOL;
	string PartyID = NOL;
	string PartyIDSource = NOL;
	string PartyRole = NOL;

};
struct DLLEXPORT CommissionData
{
	string Commission = NOL;
	string CommType = NOL;
};
struct DLLEXPORT OrderQtyData
{
	string OrderQty = NOL;
	string CashOrderQty = NOL;
};
struct DLLEXPORT TrdRegTimestamps
{
	string NoTrdRegTimestamps = NOL;
	string TrdRegTimestamp = NOL;
	string TrdRegTimestampType = NOL;
};
struct DLLEXPORT FinancialDetails
{
	string StartDate = NOL;
	string EndDate = NOL;
	string DeliveryType = NOL;
};
struct DLLEXPORT UnderlyingInstrument
{
	string UnderlyingSymbol = NOL;
};
struct DLLEXPORT UnderlyingStipulations
{
	string NoUnderlyingStips = NOL;
	string UnderlyingStipType = NOL;
	string UnderlyingStipValue = NOL;
};
struct DLLEXPORT Parties
{
	string NoPartyID = NOL;
	string PartyID = NOL;
	string PartyIDSource = NOL;
	string PartyRole = NOL;
};
struct DLLEXPORT NewOrder_sngl
{
	string ClOrdID = NOL;
	Parties Party;
	string Account = NOL;
	string MaxFloor = NOL;
	string SecondaryClOrdID = NOL;
	string NoTradingSessions = NOL;
	string TradingSessionID = NOL;
	Instrument Tool;
	string Side = NOL;
	string TransactTime = NOL;
	OrderQtyData OrdQty;
	string OrdType = NOL;
	string PriceType = NOL;
	string Price = NOL;
	string TradeThruTime = NOL;
	string TimeInForce = NOL;
	string EffectiveTime = NOL;
	string OrderCapacity = NOL;
	string OrderRestrictions = NOL;
	string MaxPriceLevels = NOL;
};

struct DLLEXPORT OrderCancel
{
	string OrigClOrdID = NOL;
	string OrderID = NOL;
	string ClOrdID = NOL;
	string Side = NOL;
	string TransactTime = NOL;
};
struct DLLEXPORT OrderTable
{
	string ClOrdID = NOL;
	string Status = NOL;
	string ExecType = NOL;
	string WorkingIndicator = NOL;
	string OrdType = NOL;
	string Side = NOL;
	string Price = NOL;
	string LeavesQty = NOL;
	string CumQty = NOL;
	Instrument Tool;
	string LastQty = NOL;
	string LastPrice = NOL;
	string TradingSessionID = NOL;
	string Time = NOL;
	string SeqNum = NOL;
};

struct DLLEXPORT RejectedRequest
{
	string OrderID = NOL;
	string OrdStatus = NOL;
	string CxlRejResponseTo = NOL;
	string Text = NOL;
	string CxlQty = NOL;
	string Time = NOL;
	string SeqNum = NOL;
	string ClOrdID;
};
using namespace std;

class DLLEXPORTNOT OrderManager
{
	RejectedRequest LastRejectedCancel;
	OrderTable LastChangedOrder;
public:
	void Process(OrderTable* Arguments);
	void Reject(RejectedRequest* Arguments);
	RejectedRequest GetReject();
	OrderTable GetChanged();
};

class DLLEXPORT SystemVariablesProcessor
{
	bool choose(string a, string b);
	bool parse(char* str);
public:
	bool logflag;
	string LOGNAME;
	ofstream LOGGING;
	SystemVariablesProcessor(string FName);
} SysVars("FIX_vars.inf");

class DLLEXPORTNOT TCPClient
{
	SOCKET sock;
	WSADATA wsDATA;
	int LastError;
public:
	bool initial(PCSTR Addr, PCSTR Port);
	int Send(PCSTR message, int len);
	int Receive(PCSTR output, string& input);
};

class DLLEXPORTNOT MessageFormatter
{
private:
	map<string, string> Heads;
	string SenderCompID, TargetCompID;
	void Load(int SeqNum);
	string Linking(list<string>* BodyOrder);
public:
	void MessageFormatterInit(string senderCompID, string targetCompID);
	string Logon(string, int, string, string);
	string HeartBeat(int, string);
	string TestRequest(int SeqNum, string TestReqID);
	string Logout(int, string);
	string NewOrder_Single(int SeqNum, NewOrder_sngl* Arguments);
	string OrderCancelRequest(int SeqNum, OrderCancel* Arguments);
};

class DLLEXPORTNOT OutputProcessor
{
private:
	TCPClient* TCP;
	int SeqNum;
	MessageFormatter* LocalFormatter;
	short Send(string Message);
public:
	short HeartBeat(string);
	short Logon(string, string, string);
	short Logout(string);
	short NewOrder_Single(NewOrder_sngl* Arguments);
	short OrderCancelRequest(OrderCancel* Arguments);
	void OutputProcessorInit(MessageFormatter* localFormatter, TCPClient* tcp);
};

class DLLEXPORTNOT SystemInputProcessor
{
	OutputProcessor* Output;
	map <string, string> Heads;
	short HeartBeat();
	short TestRequest();
	short Logon();
	short Logout();
	short Reject();
public:
	void Process(map <string, string> heads);
	void LoadOutputProcessor(OutputProcessor* Out);
};

class DLLEXPORTNOT ClientInputProcessor {
	OutputProcessor* Output;
	OrderManager* OrdMan;
	map <string, string> Heads;
	void ExecutionReport();
	void OrderCancelReject();
public:
	void Process(map <string, string> heads);
	void LoadOutputProcessor(OutputProcessor* Out, OrderManager* OrderMan);
};


class DLLEXPORTNOT InputProcessor
{
private:
	string InputString;
	SystemInputProcessor SysInProc;
	ClientInputProcessor ClInProc;
	map <string, string> Parsing();
	short Distributing();
public:
	void Receive(string& Input);
	void InputProcessorInit(OutputProcessor* Out, OrderManager* Ordman);
};

class DLLEXPORT FIX44
{
private:
	TCPClient TCP;
	MessageFormatter Formatter;
	OutputProcessor Out;
	InputProcessor In;
	thread Listener;
	string InputContainer;
	void Listening();
	OrderManager OrdMan;
public:
	short Logon(string Password, string HeartBtInt, string ResetSeqNum);
	short Logout(string Text);
	short NewOrder_Single(NewOrder_sngl* Arguments);
	short OrderCancelRequest(OrderCancel* Arguments);
	void FIX44Init(string senderCompID, string targetCompID, string address, string port);
	OrderTable LastChangedOrder();
	RejectedRequest LastRejectedCancel();
};
