// SimpleClient.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MyServer.h"
#include "SimpleClient.h"
#include "SimpleServer.h"


// CSimpleClient

IMPLEMENT_DYNAMIC(CSimpleClient, CWnd)

CSimpleClient::CSimpleClient(SOCKET clientSocket, SOCKADDR_IN clientAddr, int nConnectedID, CWnd* pParent/*=NULL*/)
{
	m_hParent = NULL;
	m_pParent = pParent;
	if(pParent)
		m_hParent = pParent->GetSafeHwnd();
	Socket = clientSocket;
	m_ClientIP = clientAddr;
	m_nClientID = nConnectedID;

	m_pReceiveBuffer = new char[BUFSIZE]; // 1mb

	CreateWndForm(WS_CHILD | WS_OVERLAPPED);
	//HWND hwnd = this->GetSafeHwnd();

	ThreadStart();
	//printf("[TCP %s : %d]  %s\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port), buf);
}

CSimpleClient::~CSimpleClient()
{
	//소켓 종료
	closesocket(Socket);			// accept() 상태를 벗어남.

	ThreadStop();
	Sleep(30);
	t1.join();

	if (m_pReceiveBuffer)
	{
		delete[] m_pReceiveBuffer;
		m_pReceiveBuffer = NULL;
	}
}


BEGIN_MESSAGE_MAP(CSimpleClient, CWnd)
END_MESSAGE_MAP()



// CSimpleClient 메시지 처리기입니다.
BOOL CSimpleClient::CreateWndForm(DWORD dwStyle)
{
	if (!Create(NULL, _T("SimpleClient"), dwStyle, CRect(0, 0, 0, 0), m_pParent, (UINT)this))
	{
		AfxMessageBox(_T("CSimpleClient::Create() Failed!!!"));
		return FALSE;
	}

	return TRUE;
}

BOOL CSimpleClient::Send(CString sSend)
{
	int nLen = sSend.GetLength() + 1; // for '\0'
	char* cSend = new char[nLen];
	StringToChar(sSend, cSend);
	int retval = send(Socket, cSend, strlen(cSend), 0);
	delete cSend;
	if (retval == SOCKET_ERROR)
	{
		AfxMessageBox(_T("send() Error"));
		return FALSE;
	}
	return TRUE;
}

void CSimpleClient::ThreadStart()
{
	m_bThreadStateEnd = FALSE;
	m_bThreadAlive = TRUE;
	t1 = std::thread(ProcThrd, this);
}

void CSimpleClient::ProcThrd(const LPVOID lpContext)
{
	CSimpleClient* pSimpleClient = reinterpret_cast<CSimpleClient*>(lpContext);

	while (pSimpleClient->ThreadIsAlive())
	{
		if (!pSimpleClient->ProcReceive())
			break;
	}

	pSimpleClient->ThreadEnd();
}

BOOL CSimpleClient::ProcReceive()
{
	int nRecSize = -1;
	char buffer[BUFSIZE] = { 0, };
	nRecSize = recv(Socket, buffer, BUFSIZE, 0);
	if (nRecSize > 0)
	{
		memcpy(m_pReceiveBuffer, buffer, nRecSize);
		m_pReceiveBuffer[nRecSize] = _T('\0');
		CString sMsg = CharToString(m_pReceiveBuffer);
		::SendMessage(m_hParent, WM_CLIENT_RECEIVED, (WPARAM)m_nClientID, (LPARAM)(LPCTSTR)sMsg);
		//if (m_pParent)
		//	((CSimpleServer*)m_pParent)->wmClientReceived((WPARAM)m_nClientID, (LPARAM)(LPCTSTR)sMsg);
		//	//((CSimpleServer*)m_pParent)->SendMessage(WM_CLIENT_RECEIVED, (WPARAM)m_nClientID, (LPARAM)(LPCTSTR)sMsg);
	}
	else if (nRecSize == 0)
	{
		// 연결 상태 끊어짐 확인.
		return FALSE;
	}

	return TRUE;
}

void CSimpleClient::ThreadStop()
{
	m_bThreadAlive = FALSE;
	MSG message;
	const DWORD dwTimeOut = 1000 * 60 * 3; // 3 Minute
	DWORD dwStartTick = GetTickCount();
	Sleep(30);
	while (!m_bThreadStateEnd)
	{
		if (GetTickCount() >= (dwStartTick + dwTimeOut))
		{
			AfxMessageBox(_T("WaitUntilThreadEnd() Time Out!!!", NULL, MB_OK | MB_ICONSTOP));
			return;
		}
		if (::PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&message);
			::DispatchMessage(&message);
		}
		Sleep(30);
	}
}

void CSimpleClient::ThreadEnd()
{
	m_bThreadStateEnd = TRUE;
	::PostMessage(m_hParent, WM_CLIENT_CLOSED, (WPARAM)m_nClientID, (LPARAM)0);
	//((CSimpleServer*)m_pParent)->wmClientClosed((WPARAM)m_nClientID, (LPARAM)0);
}

BOOL CSimpleClient::ThreadIsAlive()
{
	return m_bThreadAlive;
}


void CSimpleClient::StringToChar(CString str, char* szStr)  // char* returned must be deleted... 
{
	int nLen = str.GetLength();
	strcpy(szStr, CT2A(str));
	szStr[nLen] = _T('\0');
}

void CSimpleClient::StringToTChar(CString str, TCHAR* tszStr) // TCHAR* returned must be deleted... 
{
	int nLen = str.GetLength() + 1;
	memset(tszStr, 0x00, nLen * sizeof(TCHAR));
	_tcscpy(tszStr, str);
}

CString CSimpleClient::CharToString(char *szStr)
{
	CString strRet;

	int nLen = strlen(szStr) + sizeof(char);
	wchar_t *tszTemp = NULL;
	tszTemp = new WCHAR[nLen];

	MultiByteToWideChar(CP_ACP, 0, szStr, -1, tszTemp, nLen * sizeof(WCHAR));

	strRet.Format(_T("%s"), (CString)tszTemp);
	if (tszTemp)
	{
		delete[] tszTemp;
		tszTemp = NULL;
	}
	return strRet;
}


