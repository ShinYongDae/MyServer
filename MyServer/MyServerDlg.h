
// MyServerDlg.h : 헤더 파일
//

#pragma once

#include "SimpleServer.h"
#include "afxwin.h"


// CMyServerDlg 대화 상자
class CMyServerDlg : public CDialogEx
{
	CSimpleServer *m_pServer;

	void DispClientStatus();

// 생성입니다.
public:
	CMyServerDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.
	~CMyServerDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MYSERVER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT wmServerReceived(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnStart();
	afx_msg void OnBnClickedBtnStop();
	afx_msg void OnBnClickedBtnSend();
	afx_msg void OnBnClickedBtnClear();
	CEdit m_editMsgList;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
