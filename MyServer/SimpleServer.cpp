// SimpleServer.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MyServer.h"
#include "SimpleServer.h"


// CSimpleServer

IMPLEMENT_DYNAMIC(CSimpleServer, CWnd)

CSimpleServer::CSimpleServer(CString sServerIp, int nPort, CWnd* pParent/*=NULL*/)
{
	m_pParent = pParent;
	m_nConnectedID = -1;

	for (int i = 0; i < MAX_CLIENT; i++)
	{
		m_pClientAddr[i] = NULL;
	}

	//윈도우 소켓(원속) 초기화
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		AfxMessageBox(_T("실패 - 윈도우 소켓(원속) 초기화"));
		return;
	}

	//소켓 확인
	listenSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocket == INVALID_SOCKET)
	{
		AfxMessageBox(_T("socket() Error"));
		return;
	}

	//연결 확인(listen)
	int nLen = sServerIp.GetLength() + 1; // for '\0'
	char* cServerIp = new char[nLen];
	StringToChar(sServerIp, cServerIp);

	int recval;
	SOCKADDR_IN servAddr;
	ZeroMemory(&servAddr, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(nPort);
	servAddr.sin_addr.S_un.S_addr = inet_addr(cServerIp);
	recval = bind(listenSocket, (SOCKADDR *)&servAddr, sizeof(servAddr));
	delete cServerIp;
	if (recval == SOCKET_ERROR)
	{
		closesocket(listenSocket);
		AfxMessageBox(_T("bind() Error"));
		return;
	}
	recval = listen(listenSocket, SOMAXCONN);
	if (recval == SOCKET_ERROR)
	{
		closesocket(listenSocket);
		AfxMessageBox(_T("listen() Error"));
		return;
	}

	if (!Create(NULL, _T("Server"), WS_CHILD, CRect(0, 0, 0, 0), m_pParent, (UINT)this))
	{
		AfxMessageBox(_T("CSimpleServer::Create() Failed!!!"));
		return;
	}
	//CWnd *pP = (CWnd*)::GetParent(this->m_hWnd);

	StartThread();
	//printf("[TCP %s : %d]  %s\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port), buf);
}

CSimpleServer::~CSimpleServer()
{
	for (int i = 0; i < MAX_CLIENT; i++)
	{
		if (m_pClientAddr[i])
		{
			delete m_pClientAddr[i];
			m_pClientAddr[i] = NULL;
			Sleep(30);
		}
	}

	shutdown(listenSocket, SD_BOTH);
	closesocket(listenSocket);			// accept() 상태를 벗어남.

	StopThread();
	Sleep(30);
	t1.join();

	WSACleanup();
}


BEGIN_MESSAGE_MAP(CSimpleServer, CWnd)
	ON_MESSAGE(WM_SERVER_ACCEPT, wmServerAccept)
	ON_MESSAGE(WM_CLIENT_CLOSED, wmClientClosed)
	ON_MESSAGE(WM_CLIENT_RECEIVED, wmClientReceived)
END_MESSAGE_MAP()



// CSimpleServer 메시지 처리기입니다.

LRESULT CSimpleServer::wmServerAccept(WPARAM wParam, LPARAM lParam)
{
	SOCKET clientSocket = (SOCKET)wParam;
	SOCKADDR* pclientAddr = (SOCKADDR*)lParam;
	SOCKADDR_IN* ppclientAddr = (SOCKADDR_IN*)pclientAddr;

	for (int i = 0; i < MAX_CLIENT; i++)
	{
		if (!m_pClientAddr[i])
		{
			m_nConnectedID = i;
			m_pClientAddr[m_nConnectedID] = new CSimpleClient(clientSocket, *ppclientAddr, m_nConnectedID, this);
			break;
		}
	}

	return (LRESULT)1;
}

LRESULT CSimpleServer::wmClientReceived(WPARAM wParam, LPARAM lParam)
{
	int nClientId = (int)wParam;
	CString sReceived = (LPCTSTR)lParam;
	HWND hParentWnd = m_pParent->GetSafeHwnd();
	::SendMessage(hParentWnd, WM_SERVER_RECEIVED, (WPARAM)nClientId, (LPARAM)(LPCTSTR)sReceived);

	return (LRESULT)1;
}

LRESULT CSimpleServer::wmClientClosed(WPARAM wParam, LPARAM lParam)
{
	int nClientID = (int)wParam;

	if (m_pClientAddr[nClientID])
	{
		delete m_pClientAddr[nClientID];
		m_pClientAddr[nClientID] = NULL;
	}

	return (LRESULT)1;
}

BOOL CSimpleServer::Send(int nClientID, CString sSend)
{
	if (m_pClientAddr[nClientID])
	{
		m_pClientAddr[nClientID]->Send(sSend);
	}
	return TRUE;
}


void CSimpleServer::StartThread()
{
	m_bEndThreadState = FALSE;
	m_bAliveThread = TRUE;
	t1 = std::thread(thrdReceive, this);
}

void CSimpleServer::thrdReceive(const LPVOID lpContext)
{
	CSimpleServer* pSimpleServer = reinterpret_cast<CSimpleServer*>(lpContext);

	while (pSimpleServer->IsAliveThread())
	{
		if (!pSimpleServer->Receive())
			break;
	}

	pSimpleServer->EndThread();
}

BOOL CSimpleServer::Receive()
{
	//접속
	SOCKET clientSocket;
	SOCKADDR_IN clientAddr;
	//SOCKADDR clientAddr;
	int nlength = sizeof(clientAddr);
	char buf[BUFSIZE];

	clientSocket = accept(listenSocket, (SOCKADDR*)&clientAddr, &nlength);
	if (clientSocket == INVALID_SOCKET)
	{
		//AfxMessageBox(_T("accept() Error"));
		if (!m_bAliveThread)
			return FALSE;
	}
	else
	{
		//HWND hWnd = this->GetSafeHwnd();
		//::SendMessage(hWnd, WM_SERVER_ACCEPT, (WPARAM)clientSocket, (LPARAM)&clientAddr);

		for (int i = 0; i < MAX_CLIENT; i++)
		{
			if (!m_pClientAddr[i])
			{
				m_nConnectedID = i;
				m_pClientAddr[m_nConnectedID] = new CSimpleClient(clientSocket, clientAddr, m_nConnectedID, this);
				break;
			}
		}
	}

	return TRUE;
}

BOOL CSimpleServer::IsAliveThread()
{
	return m_bAliveThread;
}

void CSimpleServer::EndThread()
{
	m_bEndThreadState = TRUE;
}

void CSimpleServer::StopThread()
{
	m_bAliveThread = FALSE;
	MSG message;
	const DWORD dwTimeOut = 1000 * 60 * 3; // 3 Minute
	DWORD dwStartTick = GetTickCount();
	Sleep(30);
	while (!m_bEndThreadState)
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

BOOL CSimpleServer::IsConnected(int nClientID)
{
	if (nClientID >= MAX_CLIENT)
		return FALSE;

	return (m_pClientAddr[nClientID] != NULL);
}


void CSimpleServer::StringToChar(CString str, char* szStr)  // char* returned must be deleted... 
{
	int nLen = str.GetLength();
	strcpy(szStr, CT2A(str));
	szStr[nLen] = _T('\0');
}

void CSimpleServer::StringToTChar(CString str, TCHAR* tszStr) // TCHAR* returned must be deleted... 
{
	int nLen = str.GetLength() + 1;
	memset(tszStr, 0x00, nLen * sizeof(TCHAR));
	_tcscpy(tszStr, str);
}

CString CSimpleServer::CharToString(char *szStr)
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

