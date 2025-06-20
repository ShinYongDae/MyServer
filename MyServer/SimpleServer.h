#pragma once

#include "SimpleClient.h"

#define MAX_CLIENT	8


// CSimpleServer

class CSimpleServer : public CWnd
{
	DECLARE_DYNAMIC(CSimpleServer)

	CWnd* m_pParent;
	HWND m_hParent;
	BOOL m_bThreadAlive, m_bThreadStateEnd;
	std::thread t1;

	CSimpleClient* m_pClientAddr[MAX_CLIENT]; // Number of Max connection is 10...
	int m_nConnectedID;

	BOOL CreateWndForm(DWORD dwStyle);

	void StringToChar(CString str, char* szStr);
	void StringToTChar(CString str, TCHAR* tszStr);
	CString CharToString(char *szStr);
	void ThreadStart();
	void ThreadStop();

public:
	CSimpleServer(CString sServerIp, int nPort, CWnd* pParent = NULL);
	virtual ~CSimpleServer();

	char* m_pReadBuffer;
	SOCKET listenSocket;

	static void ProcThrd(const LPVOID lpContext);
	BOOL Send(int nClientID, CString sSend);
	BOOL IsConnected(int nClientID);

	afx_msg LRESULT wmServerAccept(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT wmClientReceived(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT wmClientClosed(WPARAM wParam, LPARAM lParam);

protected:
	void ThreadEnd();
	BOOL ProcReceive();
	BOOL ThreadIsAlive();

protected:
	DECLARE_MESSAGE_MAP()
};


