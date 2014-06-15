#include "StdAfx.h"
//#include "Uri.h"
#include "Lock.h"
#include "FCGIRecord.h"
#include "FCGIResponder.h"


FCGIResponder::FCGIResponder(IHTTPServer *server, IRequest* request, bool cacheAll)
	: _server(server), _request(request), _cacheAll(cacheAll), _svrCode(SC_UNKNOWN), _fcgiAdp(NULL), _httpAdp(NULL),
	_bytesHTTPSent(0), _bytesHTTPRecv(0), _bytesFCGISent(0), _bytesFCGIRecv(0),
	_recordPipe(NULL), _cacheFilePipe(NULL)
{
	_lock = new Lock();
}

FCGIResponder::~FCGIResponder()
{
	if(_lock) delete _lock;
	if(_recordPipe) delete _recordPipe;
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


void FCGIResponder::prepareFCGIInput()
{
	assert(_recordPipe == NULL);
	_recordPipe = new BufferPipe(65535, 4 * K_BYTES);

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
	_recordPipe->write(record.buffer(), record.size());
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
	_recordPipe->write(record.buffer(), record.size());
	record.reset();

	/* �ռ�¼��ʾ���� */
	record.setHeader(_fcgiId, FCGI_PARAMS);
	record.setEnd();
	_recordPipe->write(record.buffer(), record.size());
	record.reset();

	// ���ӹܵ�
	if(_request->getPostData())
	{
		// ����һ�� FCGIRecord �任��,����һ���ܵ������������ һ���� FCGIRecord ��ʽ�����ݰ���.
		_recordTransformer = new FCGITransformer(FCGI_STDIN, _fcgiId, 4096);
		build_pipes_chain(_request->getPostData(), (IPipe*)_recordTransformer, (IPipe*)_recordPipe, (IPipe*)_fcgiAdp);
	}
	else
	{
		// ���һ���� STDIN 
		record.setHeader(_fcgiId, FCGI_STDIN);
		record.setEnd();
		_recordPipe->write(record.buffer(), record.size());
		record.reset();
		build_pipes_chain((IPipe*)_recordPipe, (IPipe*)_fcgiAdp);
	}
}

// ���� FCGI ������������������
void FCGIResponder::disposeRecord(FCGIRecord* record)
{
	u_char t = record->getType();

	// ���ദ��
	if(FCGI_STDOUT == t)
	{
		// ����д�뻺��(_cacheFilePipe,_recordPipe �Ѿ�����,����ֻ�� _cacheFilePipe д�뼴��)
		size_t wr = _cacheFilePipe->write(record->getBodyData(), record->getBodyLength());
		assert(wr == record->getBodyLength());

		/* ���� HTTP Client Adp �������ݵ� HTTP �ͻ��� */
		if(!_cacheAll)
		{
			tryHttpClientPump();
		}
	}
	else if(FCGI_END_REQUEST == t)
	{
		/* ������� */
		unsigned int appStatus = 0;
		byte protocolStatus = 0;
		if(record->getEndRequestBody(appStatus, protocolStatus))
		{
			assert(protocolStatus == FCGI_REQUEST_COMPLETE);
		}
		else
		{
			assert(0);
		}

		_fcgiRequestEnd = true;

		/// �������ݵ� HTTP �ͻ���
		tryHttpClientPump();
	}
	else if(FCGI_STDERR == t)
	{
		/* д��־ */
		std::string err;
		err.assign((const char*)record->getBodyData(), record->getBodyLength());
		LOGGER_CWARNING(theLogger, _T("%s\r\n"), AtoT(err).c_str());
	}
	else
	{
		/* ���� */
		assert(0);
	}
}

void FCGIResponder::tryHttpClientPump()
{
	// ׼�� HTTP ��Ӧͷ(������Ҫ�ж��Ƿ�Ҫ���� FCGI ��������д����Ӧͷ)
	if(!_headerFormatted)
	{
		/* FCGI ������һ���ᷢ����Ӧͷ��? */
		/* ������ FCGI ���̲�������Ӧͷ�ļ���������: Status, Content-Length, Transfer-Encoding */
		int headerlen = http_header_end((const char*)_cacheBufferPipe->buffer(), (size_t)_cacheBufferPipe->size());
		if(headerlen > 0)
		{
			_cacheBufferPipe->skip(headerlen);

			/* ����һ�� HTTP ��Ӧͷ */
			_svrCode = SC_OK;
			_header.setResponseCode(_svrCode);
			_header.addDefaultFields();
			_header.add(std::string((const char*)_recordPipe->buffer(), headerlen));

			/* ������ FCGI ���̲�������Ӧͷ�ļ���������: Status, Content-Length, Transfer-Encoding */
			std::string tmp;
			if(_header.getField("Status", tmp))
			{
				// FCGI Status ��ָ���µ���Ӧ��
				_svrCode = atoi(tmp.c_str());
				if( _svrCode == 0) _svrCode = SC_OK;
				_header.setResponseCode(_svrCode);
				_header.remove("Status");
			}

			int contentLen = 0;
			if(_header.getField("Content-Length", tmp))
			{
				// FCGI ����ָ�������ݵĳ���
				contentLen = atoi(tmp.c_str());
			}
			else
			{
				if(_fcgiRequestEnd)
				{
					char clstr[100] = {0};
					contentLen = (size_t)_cacheFilePipe->size() + (size_t)_cacheBufferPipe->size();
					sprintf(clstr, "%d", contentLen);
					_header.add("Content-Length", clstr);
				}
			}

			if(!_header.getField("Transfer-Encoding", tmp) && contentLen == 0)
			{
				// FCGI ������û��ָ������,����û��ָ�� Transfer-Encoding ��ʹ�� chunked ����.
				_header.add("Transfer-Encoding", "chunked");
				_chunkTransformer = new ChunkTransformer(4 * K_BYTES);
			}

			/* �Ƿ񱣳����� */
			if(_request->keepAlive())
			{
				_header.add("Connection", "keep-alive");
			}
			else
			{
				_header.add("Connection", "close");
			}

			/* ��ʽ����Ӧͷ׼����� */
			_header.format();
			_headerFormatted = true;

			// ���ӹܵ�
			if(_chunkTransformer)
			{
				build_pipes_chain((IPipe*)_cacheFilePipe, (IPipe*)_cacheBufferPipe, (IPipe*)_chunkTransformer, (IPipe*)&_header);
			}
			else
			{
				build_pipes_chain((IPipe*)_cacheFilePipe, (IPipe*)_cacheBufferPipe, (IPipe*)&_header);
			}
		}
	}

	// �� _cacheFilePipe -> _cacheBufferPipe -> _header �Ĺܵ����г�ȡ����
	_httpAdp->pump(0, &_header);
}

bool FCGIResponder::beforeStep(IOAdapter* adp, int ev, stm_result_t* res)
{
	if(TEST_BIT(ev, IO_EVENT_EPOLLERR))
	{
		res->rc = STM_ABORT;
		res->param.errorCode = SE_NETWORKFAILD;
		return false;
	}

	if(TEST_BIT(ev, IO_EVENT_EPOLLHUP))
	{
		res->rc = STM_ABORT;
		res->param.errorCode = SE_REMOTEFAILD;
		return false;
	}
	return true;
}

// ��ʼ�� FCGI ��������
bool FCGIResponder::step0(IOAdapter* adp, int ev, stm_result_t* res)
{
	lock();
	if(adp != _fcgiAdp)
	{
		// HTTP���ӱ���ȵ� FCGI �����Լ�׼������֮��ſ���
		_httpAdp = adp;
		res->rc = STM_PAUSE;
	}
	else
	{
		/*
		* ��д FCGI ����,��������ܵ�
		*/
		prepareFCGIInput();

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
			assert(_recordPipe->size() == 0);

			// ��������ܵ�
			build_pipes_chain((IPipe*)_fcgiAdp, (IPipe*)_recordPipe);

			// �����������
			// ����ֱ�Ӱ�����������ӵ�����ܵ���ԭ���� 
			/// 1. FCGI ���������������������� FCGI Record ��ʽ,��Ҫ����(����ͨ�����һ���任������������); 
			/// 2. ����Ҫ���ǲ����������ݶ����͵��ͻ���,���߼�����˵��ֱ�����ӱȽϺ�һЩ.
			build_pipes_chain((IPipe*)_cacheFilePipe, (IPipe*)_cacheBufferPipe);

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

	if(adp != _fcgiAdp)
	{
		// ���Դӻ����ڶ�ȡ���ݷ��͵� HTTP �ͻ���
		tryHttpClientPump();
	}
	else
	{
		if(0 == _recordPipe->pump())
		{
			assert(0);
			
			/// �޷��� fcgiAdp �ж�ȡ����,ͨ����һ���쳣״��.
			forward();
			res->rc = STM_ABORT;
		}
		else
		{
			FCGIRecord record;
			for(;;)
			{
				size_t recrodLen = record.assign(_recordPipe->buffer(), (size_t)_recordPipe->size());
				if(recrodLen <= 0)
				{
					// �����ڵ����ݲ�������һ�������� FCIG Record
					break;
				}
				else
				{
					// �ɹ���ȡһ�� FCGI Record
					_recordPipe->skip(recrodLen);

					// ����֮
					disposeRecord(&record);
					record.reset();
				}
			}

			if(_fcgiRequestEnd)
			{
				assert(_recordPipe->size() == 0);

				/// ��Ӧ�� fcgiAdp ��״̬�����н���
				forward();
				res->rc = STM_DONE;
			}
			else
			{
				/// �������� FCGI ���������
				res->rc = STM_CONTINUE;
				res->param.epollEvent = IO_EVENT_EPOLLIN;
			}
		}
	}

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