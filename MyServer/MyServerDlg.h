
// MyServerDlg.h : ��� ����
//

#pragma once

#include "SimpleServer.h"
#include "afxwin.h"


// CMyServerDlg ��ȭ ����
class CMyServerDlg : public CDialogEx
{
	CSimpleServer *m_pServer;

	void DispClientStatus();

// �����Դϴ�.
public:
	CMyServerDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.
	~CMyServerDlg();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MYSERVER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.


// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
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
