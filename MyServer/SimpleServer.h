#pragma once

#include "SimpleClient.h"

#define MAX_CLIENT	8


// CSimpleServer

class CSimpleServer : public CWnd
{
	DECLARE_DYNAMIC(CSimpleServer)

	CWnd* m_pParent;
	BOOL m_bAliveThread, m_bEndThreadState;
	std::thread t1;

	CSimpleClient* m_pClientAddr[MAX_CLIENT]; // Number of Max connection is 10...
	int m_nConnectedID;

	void StringToChar(CString str, char* szStr);
	void StringToTChar(CString str, TCHAR* tszStr);
	CString CharToString(char *szStr);
	void StartThread();
	void StopThread();

public:
	CSimpleServer(CString sServerIp, int nPort, CWnd* pParent = NULL);
	virtual ~CSimpleServer();

	HWND m_hParentWnd;
	char* m_pReadBuffer;
	SOCKET listenSocket;

	static void thrdReceive(const LPVOID lpContext);
	BOOL Send(int nClientID, CString sSend);
	BOOL IsConnected(int nClientID);

	afx_msg LRESULT wmServerAccept(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT wmClientReceived(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT wmClientClosed(WPARAM wParam, LPARAM lParam);

protected:
	void EndThread();
	BOOL Receive();
	BOOL IsAliveThread();

protected:
	DECLARE_MESSAGE_MAP()
};


