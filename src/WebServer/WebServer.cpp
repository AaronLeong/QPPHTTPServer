// WebServer.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"

#include "MainFrm.h"
#include "WebServer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CWebServerApp

BEGIN_MESSAGE_MAP(CWebServerApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CWebServerApp::OnAppAbout)
END_MESSAGE_MAP()


// CWebServerApp ����

CWebServerApp::CWebServerApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CWebServerApp ����

CWebServerApp theApp;

// CWebServerApp ��ʼ��

BOOL CWebServerApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	// ��ʼ��WinSock 2.2 �Ͱ汾Ҳ����.
	WSADATA ws;
	WORD wVer = MAKEWORD(2, 2);
	if(0 != WSAStartup(wVer, &ws))
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
	}
	TRACE(_T("Winsock version %d.%d initialized.\r\n"), LOBYTE(ws.wVersion), HIBYTE(ws.wVersion));

	// ��Ҫ���������ڣ��˴��뽫�����µĿ�ܴ���
	// ����Ȼ��������ΪӦ�ó���������ڶ���
	CMainFrame* pFrame = new CMainFrame;
	if (!pFrame) return FALSE;
	m_pMainWnd = pFrame;

	// ���������ؿ�ܼ�����Դ
	pFrame->LoadFrame(IDR_MAINFRAME, WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL, NULL);

	if ( _tcsicmp(this->m_lpCmdLine, _T("hide")) == 0)
	{
		pFrame->ShowWindow(SW_HIDE);
		
	}
	else
	{
		pFrame->ShowWindow(this->m_nCmdShow);
	}
	
	pFrame->UpdateWindow();

	return TRUE;

}

int CWebServerApp::ExitInstance()
{
	// TODO: �ڴ����ר�ô����/����û���
	WSACleanup();
	return CWinApp::ExitInstance();
}

// CWebServerApp ��Ϣ�������




// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// �������жԻ����Ӧ�ó�������
void CWebServerApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// CWebServerApp ��Ϣ�������
