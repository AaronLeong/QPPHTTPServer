#include "StdAfx.h"
//#include "Uri.h"
#include "Lock.h"
#include "FCGIRecord.h"
#include "FCGIResponder.h"


FCGIResponder::FCGIResponder(IHTTPServer *server, IRequest* request, bool cacheAll)
	: _server(server), _request(request), _cacheAll(cacheAll), _svrCode(SC_UNKNOWN), _fcgiAdp(NULL), _httpAdp(NULL),
	_bytesHTTPSent(0), _bytesHTTPRecv(0), _bytesFCGISent(0), _bytesFCGIRecv(0),
	_fcgiBufferPipe(NULL), _cacheFilePipe(NULL)
{
	_lock = new Lock();
}

FCGIResponder::~FCGIResponder()
{
	if(_lock) delete _lock;
	if(_fcgiBufferPipe) delete _fcgiBufferPipe;
	if(_cacheFilePipe) delete _cacheFilePipe;
}

void FCGIResponder::lock()
{
	if(_lock) _lock->lock();
}

void FCGIResponder::unlock()
{
	if(_lock) _lock->unlock();
}

IOAdapter* FCGIResponder::setFCGIConnection(u_short fcgiId, IOAdapter* adp)
{
	IOAdapter* oldAdp = _fcgiAdp;
	_fcgiAdp = adp;
	_fcgiId = fcgiId;
	return oldAdp;
}

IRequest* FCGIResponder::getRequest()
{
	return _request;
}

std::string FCGIResponder::getHeader()
{
	return _header.getHeader();
}

int FCGIResponder::getServerCode()
{
	return _svrCode;
}

void FCGIResponder::statistics(__int64* bytesRecved, __int64* bytesSent)
{
	*bytesRecved = _bytesHTTPRecv;
	*bytesSent = _bytesHTTPSent;
}


void FCGIResponder::initFCGIEnv()
{
	assert(_fcgiBufferPipe);
	
	/* 
	* ׼������ 
	*/
	std::string tmp;
	char numberBuf[50] = {0};

	/* ����uri��Ϣ */
	std::string uri = _request->uri(true);
	std::string uriPath(""), uriQueryString("");
	std::string uriServerPath;
	std::string::size_type pos = uri.find('?');
	if(pos == std::string::npos)
	{
		uriPath = uri;
	}
	else
	{
		uriPath = uri.substr(0, pos);
		uriQueryString = uri.substr(pos + 1, uri.size() - pos - 1);
	}

	if(!_server->mapServerFilePath(uriPath, uriServerPath))
	{
		assert(0);
	}

	/* ��ȡ���ӵ�ַ */
	char serverIP[MAX_IP_LENGTH] = {0}, clientIP[MAX_IP_LENGTH] = {0};
	u_short serverPort = 0, clientPort = 0;
	_httpAdp->getsockname(serverIP, serverPort);
	_httpAdp->getpeername(clientIP, clientPort);

	/* HTTP���� */
	char method[50] = {0};
	map_method(_request->method(), method);

	/* SERVER_NAME - HOST */
	std::string hostName = _request->field("Host");
	if( hostName.size() <= 0)
	{
		hostName = _server->ip();
	}
	else
	{
		std::string::size_type pos = hostName.find(':');
		if(pos != std::string::npos)
		{
			hostName = hostName.substr(0, pos);
		}
	}

	/* 
	* ׼�������� 
	*/
	FCGIRecord record;

	/* ���� FCGI_BEGIN_REQUEST */
	record.setHeader(_fcgiId, FCGI_BEGIN_REQUEST);
	record.setBeginRequestBody(FCGI_RESPONDER, true);
	record.setEnd();
	_fcgiBufferPipe->write(record.buffer(), record.size());
	record.reset();

	/* ���Ͳ��� */
	record.setHeader(_fcgiId, FCGI_PARAMS);
	record.addNameValuePair("HTTPS", "off");
	record.addNameValuePair("REDIRECT_STATUS", "200");
	record.addNameValuePair("SERVER_PROTOCOL", "HTTP/1.1");
	record.addNameValuePair("GATEWAY_INTERFACE", "CGI/1.1");
	record.addNameValuePair("SERVER_SOFTWARE", SERVER_SOFTWARE);
	record.addNameValuePair("SERVER_NAME", hostName.c_str());
	record.addNameValuePair("SERVER_ADDR", serverIP);
	record.addNameValuePair("SERVER_PORT", itoa(serverPort, numberBuf, 10));
	record.addNameValuePair("REMOTE_ADDR", clientIP);
	record.addNameValuePair("REMOTE_PORT", itoa(clientPort, numberBuf, 10));
	record.addNameValuePair("REQUEST_METHOD", method);
	record.addNameValuePair("REQUEST_URI", uri.c_str());
	if(uriQueryString.size() > 0)
	{
		record.addNameValuePair("QUERY_STRING",uriQueryString.c_str());
	}

	record.addNameValuePair("DOCUMENT_ROOT", _server->docRoot().c_str());
	record.addNameValuePair("SCRIPT_NAME", uriPath.c_str());
	record.addNameValuePair("SCRIPT_FILENAME", uriServerPath.c_str());
	record.addNameValuePair("HTTP_HOST", _request->field("Host").c_str());
	record.addNameValuePair("HTTP_USER_AGENT", _request->field("User-Agent").c_str());
	record.addNameValuePair("HTTP_ACCEPT", _request->field("Accept").c_str());
	record.addNameValuePair("HTTP_ACCEPT_LANGUAGE", _request->field("Accept-Language").c_str());
	record.addNameValuePair("HTTP_ACCEPT_ENCODING", _request->field("Accept-Encoding").c_str());

	tmp = _request->field("Cookie");
	if(tmp.size() > 0)
	{
		record.addNameValuePair("HTTP_COOKIE", tmp.c_str());
	}
	
	tmp = _request->field("Referer");
	if(tmp.size() > 0)
	{
		record.addNameValuePair("HTTP_REFERER", tmp.c_str());
	}

	tmp = _request->field("Content-Type");
	if(tmp.size() > 0)
	{
		record.addNameValuePair("CONTENT_TYPE", tmp.c_str());
	}

	tmp = _request->field("Content-Length");
	if(tmp.size() > 0)
	{
		record.addNameValuePair("CONTENT_LENGTH", tmp.c_str());
	}
	record.setEnd();
	_fcgiBufferPipe->write(record.buffer(), record.size());
	record.reset();

	/* �ռ�¼��ʾ���� */
	record.setHeader(_fcgiId, FCGI_PARAMS);
	record.setEnd();
	_fcgiBufferPipe->write(record.buffer(), record.size());
	record.reset();
}

// ��ʼ�� FCGI ��������
bool FCGIResponder::step0(IOAdapter* adp, int ev, stm_result_t* res)
{
	lock();
	if(adp != _fcgiAdp)
	{
		assert(_httpAdp == NULL);
		// HTTP���ӱ���ȵ� FCGI �����Լ�׼������֮��ſ���
		_httpAdp = adp;
		res->rc = STM_PAUSE;
	}
	else
	{
		/*
		* ��ʼ�� FCGI ����
		*/
		// һ�� FCGIRecord ��󳤶���2���ֽ�,�� 65535
		_fcgiBufferPipe = new BufferPipe(65535, 4 * K_BYTES);
		initFCGIEnv();

		// ����һ�� FCGIRecord �任��,����һ���ܵ������������ һ���� FCGIRecord ��ʽ�����ݰ���.
		_fcgiTransformer = new FCGITransformer(FCGI_STDIN, _fcgiId, 4096);

		// ���ӹܵ�
		if(_request->getPostData())
		{
			build_pipes_chain(_request->getPostData(), (IPipe*)_fcgiTransformer, (IPipe*)_fcgiBufferPipe, (IPipe*)_fcgiAdp);
		}
		else
		{
			build_pipes_chain((IPipe*)_fcgiTransformer, (IPipe*)_fcgiBufferPipe, (IPipe*)_fcgiAdp);
		}

		// ��ʼ���͵� FCGI ������
		res->rc = STM_CONTINUE;
		res->param.epollEvent = IO_EVENT_EPOLLOUT;
		forward();
	}
	unlock();
	return false;
}

// FCGIResponder -> FCGI Server: ���� FCGI ��������, HTTP POST
bool FCGIResponder::step1(IOAdapter* adp, int ev, stm_result_t* res)
{
	// �����껷��������,����� HTTP POST ����,��������֮.
	lock();
	
	if(adp != _fcgiAdp)
	{
		assert(_httpAdp == NULL);
		_httpAdp = adp;
		res->rc = STM_PAUSE;
	}
	else
	{
		if(_fcgiAdp->pump() == 0)
		{
			// ��ʼ���� FCGI server ����Ӧ����
			forward();
			res->rc = STM_CONTINUE;
			res->param.epollEvent = IO_EVENT_EPOLLIN;
		}
		else
		{
			// �������͵� FCGI Server
			res->rc = STM_CONTINUE;
			res->param.epollEvent = IO_EVENT_EPOLLOUT;
		}
	}

	unlock();
	return false;
}

// FCGI Server -> FCGIResponder:��ʽ��������֮ǰ,׼�����͵�HTTP�ͻ��˵���Ӧͷ
bool FCGIResponder::step2(IOAdapter* adp, int ev, stm_result_t* res)
{
	lock();
	unlock();
	return false;
}

// FCGI Server -> FCGIResponder: ��ʽ��������
// FCGI Responder -> HTTP Client
bool FCGIResponder::step3(IOAdapter* adp, int ev, stm_result_t* res)
{
	lock();
	unlock();
	return false;
}

// ����
bool FCGIResponder::step4(IOAdapter* adp, int ev, stm_result_t* res)
{
	lock();
	unlock();
	return false;
}