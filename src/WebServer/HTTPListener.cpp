#include "StdAfx.h"
#include "HTTPListener.h"
#include "IOSelector.h"

HTTPListener::HTTPListener()
	: _newAdp(NULL)
{
}


HTTPListener::~HTTPListener()
{
}

IOAdapter* HTTPListener::getAdapter()
{
	return _newAdp;
}

bool HTTPListener::step0(IOAdapter* adp, int ev, stm_result_t* res)
{
	if(TEST_BIT(ev, IO_EVENT_EPOLLERR) || TEST_BIT(ev, IO_EVENT_EPOLLHUP))
	{
		res->rc = STM_ABORT;
		res->param.errorCode = SE_NETWORKFAILD;
		return false;
	}

	_newAdp = adp->accept();
	if(_newAdp)
	{
		res->rc = STM_STEPDONE;
		res->param.epollEvent = IO_EVENT_EPOLLIN;
	}
	else
	{
		/* �������Ժ�,remote �˹ر����׽���WSAECONNRESET������,ֱ�Ӻ��� */
		LOGGER_CWARNING(theLogger, _T("��׽��WSAECONNRESET,������[%d].\r\n"), WSAGetLastError());

		res->rc = STM_CONTINUE;
		res->param.epollEvent = IO_EVENT_EPOLLIN;
	}
	return false;
}
