// stdafx.cpp : ֻ������׼�����ļ���Դ�ļ�
// WebServer.pch ����ΪԤ����ͷ
// stdafx.obj ������Ԥ����������Ϣ

#include "stdafx.h"



#define MY_REGKEY _T("httpserver")
BOOL AutoLaunch(BOOL bRun /* = TRUE */)
{	
	// ��ȡ·��
	TCHAR szFileName[MAX_PATH + 10] = {0};
	szFileName[0] = _T('\"');
	if (0 == GetModuleFileName(NULL, szFileName + 1, MAX_PATH) ) return FALSE;
	_tcscat(szFileName, _T("\" hide")); // ��С������

	BOOL bRet = FALSE;
	HKEY hKey;
	LPCTSTR szKeyPath = _T("Software\\Microsoft\\Windows\\CurrentVersion");		
	long ret = RegOpenKeyEx(HKEY_CURRENT_USER, szKeyPath, 0, KEY_WRITE, &hKey);
	if(ret != ERROR_SUCCESS)
	{
		TRACE("�޷���ȡע���.");
	}
	else
	{
		HKEY hRunKey;
		ret = RegCreateKeyEx(hKey, _T("Run"), 0, NULL, 0, KEY_WRITE, NULL, &hRunKey, NULL);
		if(ERROR_SUCCESS == ret)
		{
			if(bRun)
			{
				bRet = (ERROR_SUCCESS == ::RegSetValueEx(hRunKey, MY_REGKEY, 0, REG_SZ, (BYTE*)szFileName, (_tcslen(szFileName) + 1) * sizeof(TCHAR)));
			}
			else
			{
				ret = RegDeleteValue(hRunKey, MY_REGKEY);
				bRet = (ret == ERROR_SUCCESS);
			}

			RegCloseKey(hRunKey);
		}
		else
		{
			TRACE("�޷�дע���.");
		}
		RegCloseKey(hKey);
	}
	return bRet;
}


BOOL IsAutoLaunch()
{
	BOOL bRet = FALSE;
	HKEY hKey;
	TCHAR szKeyPath[] = _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run");		
	long ret = RegOpenKeyEx(HKEY_CURRENT_USER, szKeyPath, 0, KEY_READ, &hKey);
	if(ret != ERROR_SUCCESS)
	{
		TRACE("�޷���ȡע���.");
	}
	else
	{
		TCHAR szValue[MAX_PATH + 1] = {0};
		DWORD dwType = REG_SZ;
		DWORD dwLen = MAX_PATH * sizeof(TCHAR);

		LONG lRet = ::RegQueryValueEx(hKey, MY_REGKEY, NULL, &dwType, (LPBYTE)szValue, &dwLen);
		if(ERROR_SUCCESS != lRet)
		{
		}
		else
		{
			TCHAR szFileName[MAX_PATH + 10] = {0};
			if (0 == GetModuleFileName(NULL, szFileName + 1, MAX_PATH) )
			{
				TRACE("�޷���ѯ��ȡ��ǰ���̵��ļ���.");
			}
			else
			{
				szFileName[0] = _T('\"');
				_tcscat(szFileName, _T("\" hide"));
				return _tcsicmp(szFileName, szValue) == 0;
			}

		}
		RegCloseKey(hKey);
	}

	return bRet;
}

size_t GetSystemProcessorNumber()
{
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	return sysInfo.dwNumberOfProcessors;
}