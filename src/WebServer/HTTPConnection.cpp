#include "StdAfx.h"
#include "HTTPConnection.h"
#include "HTTPRequest.h"

HTTPConnection::HTTPConnection(IHTTPServer* svr, IOAdapter* adp)
	: _hrt(true), _svr(svr), _request(NULL), _responder(NULL), _factory(NULL), _adp(adp)
{
	adp->getpeername(_remoteIPAddr, _remotePort);
}

HTTPConnection::~HTTPConnection()
{
	if(_request) delete _request;
	if(_responder) _factory->releaseResponder(_responder);
}

const char* HTTPConnection::getRemoteIPAddr()
{
	return _remoteIPAddr;
}

u_short HTTPConnection::getRemotePort()
{
	return _remotePort;
}

IRequest* HTTPConnection::getRequest()
{
	return _request;
}

IResponder* HTTPConnection::getResponder()
{
	return _responder;
}

void HTTPConnection::requestBegin()
{
	_reqCount++;
	_reqStartTime = _hrt.now();
	_reqEndTime = _reqStartTime;

	_reqRecvBytes = 0;
	_reqSentBytes = 0;
}

void HTTPConnection::requestEnd()
{
	_reqEndTime = _hrt.now();

	__int64 recved = 0, sent = 0;
	_request->statistics(&recved, &sent);
	_reqRecvBytes += recved;
	_reqSentBytes += sent;

	if(_responder)
	{
		_responder->statistics(&recved, &sent);
		_reqRecvBytes += recved;
		_reqSentBytes += sent;
	}

	_connRecvBytes += _reqRecvBytes;
	_connSentBytes += _reqSentBytes;
}

void HTTPConnection::connectionBegin()
{
	_reqCount = 0;
	_connStartTime = _hrt.now();
	_connEndTime = _connStartTime;

	_connRecvBytes = 0;
	_connSentBytes = 0;
}

void HTTPConnection::connectionEnd()
{
	_connEndTime = _hrt.now();
}

bool HTTPConnection::beforeStep(IOAdapter* adp, int ev, stm_result_t* res)
{
	return true;
}


bool HTTPConnection::step0(IOAdapter* adp, int ev, stm_result_t* res)
{
	// ��¼���ӿ�ʼ��ʱ��
	connectionBegin();

	// ����һ�� HTTPRequest,׼������
	_request = new HTTPRequest(_svr);

	// ��ʼ����һ��HTTP����,��¼����ʼ��ʱ��
	requestBegin();

	forward();
	res->rc = STM_CONTINUE;
	res->param.epollEvent = IO_EVENT_EPOLLIN;
	return false;
}

bool HTTPConnection::step1(IOAdapter* adp, int ev, stm_result_t* res)
{
	// HTTPRequest ����
	assert(_request);
	_request->run(adp, ev, res);

	if(STM_DONE == res->rc)
	{
		// HTTP����������,���� IHTTPServer �ķ�������һ�� HTTP��Ӧ����.
		assert(_responder == NULL);
		_svr->catchRequest(_request, &_responder, &_factory);
		assert(_responder);

		// ���� STEPDONE ��SERVER�ṩһ����������ʼ��ʱ��
		res->st = 1;
		res->rc = STM_STEPDONE;
		res->param.epollEvent = IO_EVENT_EPOLLOUT;
		forward();
	}
	else if(STM_CONTINUE == res->rc)
	{
	}
	else if(STM_SLEEP == res->rc)
	{
	}
	else if(STM_ABORT == res->rc)
	{
		requestEnd();
		connectionEnd();

		// ��ֹ,���ӹر�
	}
	else if(STM_STEPDONE ==res->rc)
	{
		assert(0);
	}
	else
	{
		assert(0);
	}
	return false;
}

bool HTTPConnection::step2(IOAdapter* adp, int ev, stm_result_t* res)
{
	// IResponder ����
	assert(_responder);
	_responder->run(adp, ev, res);

	if(STM_DONE == res->rc)
	{
		requestEnd();

		// ����STEPDONE,SERVER���Դ���һ����������ϵ�ʱ��.
		res->rc = STM_STEPDONE;
		res->param.epollEvent = IO_EVENT_EPOLLOUT;
		res->st = 2;
		forward();
	}
	else if(STM_CONTINUE == res->rc)
	{
	}
	else if(STM_SLEEP == res->rc)
	{
	}
	else if(STM_ABORT == res->rc)
	{
		requestEnd();
		connectionEnd();

		// ��ֹ,���ӹر�
	}
	else if(STM_STEPDONE ==res->rc)
	{
	}
	else
	{
		assert(0);
	}
	return false;
}

bool HTTPConnection::step3(IOAdapter* adp, int ev, stm_result_t* res)
{
	// һ�����������
	if(_request->keepAlive())
	{
		_factory->releaseResponder(_responder);
		_factory = NULL;
		_responder = NULL;

		// ����������һ������ͷ(״̬������2����step1)
		_request->nextRequest();

		// ��ʼ����һ��HTTP����,��¼����ʼ��ʱ��
		requestBegin();

		res->rc = STM_CONTINUE;
		res->param.epollEvent = IO_EVENT_EPOLLIN;
		
		backward(2);
	}
	else
	{
		// �ر��׽���,�ȴ�shutdown�¼�ȷ�Ϸ��ͻ������ڵ������Ѿ�ȫ���������.
		adp->shutdown(SD_BOTH);

		res->rc = STM_CONTINUE;
		res->param.epollEvent = IO_EVENT_EPOLLERR;

		forward();
	}
	return false;
}

bool HTTPConnection::step4(IOAdapter* adp, int ev, stm_result_t* res)
{
	// �ر�����
	connectionEnd();

	if(TEST_BIT(ev, IO_EVENT_EPOLLERR) && adp->getLastError() == IO_ERROR_SHUTDOWN)
	{
		res->rc = STM_DONE;
	}
	else
	{
		res->rc = STM_ABORT;
		res->param.errorCode = TEST_BIT(ev, IO_EVENT_EPOLLERR) ? SE_NETWORKFAILD : SE_REMOTEFAILD;
	}
	return false;
}

void HTTPConnection::queryLastRequest(request_statis_t* info)
{
	assert(_request);
	info->uri = _request ? _request->uri(true) : "{null}";
	info->method = _request ? _request->method() : METHOD_UNDEFINE;
	info->sc = _responder ? _responder->getServerCode() : SC_UNKNOWN;
	info->usedTime = (size_t)_hrt.getMs(_reqEndTime - _reqStartTime);
	info->bytesSent = _reqSentBytes;
	info->bytesRecved = _reqRecvBytes;
}

void HTTPConnection::query(connection_statis_t* info)
{
	info->requestCount = _reqCount;
	info->usedTime = (size_t)_hrt.getMs(_connEndTime - _connStartTime);
	info->bytesRecved = _connRecvBytes;
	info->bytesSent = _connSentBytes;
}