#pragma once

#include <thread>
#include <WinSock2.h>

#define WM_CLIENT_CLOSED	(WM_USER + 0x73FC)	// 0x7FFC (Range of WM_USER is 0x400 ~ 0x7FFF)
#define WM_SERVER_ACCEPT	(WM_USER + 0x73FD)	// 0x7FFD (Range of WM_USER is 0x400 ~ 0x7FFF)
#define WM_CLIENT_RECEIVED	(WM_USER + 0x73FE)	// 0x7FFE (Range of WM_USER is 0x400 ~ 0x7FFF)
#define WM_SERVER_RECEIVED	(WM_USER + 0x73FF)	// 0x7FFF (Range of WM_USER is 0x400 ~ 0x7FFF)

#define BUFSIZE 16384

// CSimpleClient

class CSimpleClient : public CWnd
{
	DECLARE_DYNAMIC(CSimpleClient)

	CWnd* m_pParent;
	HWND m_hParent;
	SOCKADDR_IN m_ClientIP;
	int m_nClientID;

	char* m_pReceiveBuffer;
	BOOL m_bThreadAlive, m_bThreadStateEnd;
	std::thread t1;

	BOOL CreateWndForm(DWORD dwStyle);

	void StringToChar(CString str, char* szStr);
	void StringToTChar(CString str, TCHAR* tszStr);
	CString CharToString(char *szStr);
	void ThreadStart();
	void ThreadStop();

public:
	CSimpleClient(SOCKET clientSocket, SOCKADDR_IN clientAddr, int nConnectedID, CWnd* pParent = NULL);
	virtual ~CSimpleClient();

	SOCKET Socket;

	static void ProcThrd(const LPVOID lpContext);
	BOOL Send(CString sSend);

protected:
	BOOL ThreadIsAlive();
	void ThreadEnd();
	BOOL ProcReceive();

protected:
	DECLARE_MESSAGE_MAP()
};


