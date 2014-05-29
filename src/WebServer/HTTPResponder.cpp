/* Copyright (C) 2011 ������
 *
 * ����һ����Դ������,���������ɵ��޸ĺͷ���.
 * ��ֹ������ҵ��;.
 *
 * ��ϵԭ����: querw@sina.com 
*/

#include "StdAfx.h"
#include <sys/types.h>
#include <sys/stat.h>
#include "HTTPResponder.h"
#include "HTTPContent.h"

HTTPResponder::HTTPResponder(IHTTPServer *server, IRequest *req)
	: _server(server), _request(req), _content(NULL), _svrCode(SC_OK), _bytesSent(0), _bytesRecved(0)
{
	
}

HTTPResponder::HTTPResponder(IHTTPServer *server, IRequest *req, int sc, const char* msg)
	:_svrCode(sc), _server(server), _request(req)
{
	_content = new HTTPContent();
	_content->open(msg, strlen(msg), CONTENT_TYPE_TEXT);
}

HTTPResponder::~HTTPResponder()
{
	if(_content) delete _content;
}

bool HTTPResponder::makeResponseHeader(int svrCode)
{
	/* д��Ӧͷ */
	_header.setResponseCode(svrCode);
	_header.addDefaultFields();
	if(svrCode == SC_BADMETHOD)
	{
		_header.add("Allow", "GET, HEAD, POST, PUT");
	}
	if(_content)
	{
		// Last-Modified
		_header.add("Last-Modified", _content->lastModified());
		
		// ETag
		_header.add("ETag", _content->etag());
		
		// Content-Type
		_header.add("Content-Type", _content->contentType());
		
		// Content-Length
		char szLen[200] = {0};
		__int64 lLen = _content->contentLength();
		_header.add("Content-Length", _i64toa(lLen, szLen, 10));
		
		// Content-Range: bytes %d-%d/%d\r\n"
		if(SC_PARTIAL == svrCode)
		{
			_header.add("Content-Range", _content->contentRange());
		}

		// "Accept-Ranges: bytes" ֧�ֶϵ�����(ֻ�о�̬�ļ�֧�ֶϵ�����).
		if(_content->acceptRanges())
		{
			_header.add("Accept-Ranges", "bytes");
		}
	}
	else
	{
		// Content-Length 
		_header.add("Content-Length", "0");
	}
	if(_request->keepAlive())
	{
		_header.add("Connection", "keep-alive");
	}
	else
	{
		_header.add("Connection", "close");
	}

	// ��ʽ����Ӧͷ׼�����
	_header.format();

	return true;
}

std::string HTTPResponder::getHeader()
{
	return _header.getHeader();
}

int HTTPResponder::getServerCode()
{
	return _header.getResponseCode();
}

IRequest* HTTPResponder::getRequest()
{
	return _request;
}

void HTTPResponder::statistics(__int64* bytesRecved, __int64* bytesSent)
{
	*bytesRecved = _bytesRecved;
	*bytesSent = _bytesSent;
}

bool HTTPResponder::beforeStep(IOAdapter* adp, int ev, stm_result_t* res)
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

bool HTTPResponder::step0(IOAdapter* adp, int ev, stm_result_t* res)
{
	/* ��ʼ�� */
	do
	{
		if(_content != NULL)
		{
			// �Ѿ���ʼ������
			break;
		}

		// ���� request ����һ����Ӧ
		_content = new HTTPContent();

		/* �Ƿ�����ͷ��������ͷ̫�� */
		if(!_request->isValid())
		{
			_svrCode = SC_BADREQUEST;
			_content->open(g_HTTP_Bad_Request, strlen(g_HTTP_Bad_Request), CONTENT_TYPE_TEXT);
			break;
		}

		/* ��̬�ļ�ֻ֧�� GET �� HEAD ���� */
		if( METHOD_GET != _request->method() && METHOD_HEAD != _request->method())
		{
			_svrCode = SC_BADMETHOD;
			_content->open(g_HTTP_Bad_Method, strlen(g_HTTP_Bad_Method), CONTENT_TYPE_TEXT);
			break;
		}

		/* ���ļ� */
		std::string uri = _request->uri(true);
		std::string serverFilePath("");

		// ӳ��Ϊ�������ļ���.
		if(!_server->mapServerFilePath(uri, serverFilePath))
		{
			// �޷�ӳ��������ļ���,��ʾ��ֹ
			_svrCode = SC_FORBIDDEN;
			_content->open(g_HTTP_Forbidden, strlen(g_HTTP_Forbidden), CONTENT_TYPE_TEXT);
		}
		else if(serverFilePath.back() == '\\')
		{
			if(_content->open(uri, serverFilePath))
			{
				_svrCode = SC_OK;
			}
			else
			{
				_svrCode = SC_SERVERERROR;
				_content->open(g_HTTP_Server_Error, strlen(g_HTTP_Server_Error), CONTENT_TYPE_TEXT);
			}
		}
		else
		{
			// �ͻ����Ƿ������˶ϵ�����������
			// �����ļ����ݶ��󲢹�����Response����
			__int64 lFrom = 0;
			__int64 lTo = -1;
			if(_request->range(lFrom, lTo))
			{
				_svrCode = SC_PARTIAL;
			}

			if(!_content->open(serverFilePath, lFrom, lTo))
			{
				_svrCode = SC_NOTFOUND;
				_content->open(g_HTTP_Content_NotFound, strlen(g_HTTP_Content_NotFound), CONTENT_TYPE_TEXT);
			}
		}
	}
	while(false);
	
	/*
	* �����ܵ���
	*/
	makeResponseHeader(_svrCode);
	if(_request->method() == METHOD_HEAD)
	{
		_content->close();
		delete _content;
		_content = NULL;

		build_pipes_chain((IPipe*)&_header, (IPipe*)adp);
	}
	else
	{
		// ������� (IPipe*) ����ᵼ�¿ɱ������ȡʧ��.
		build_pipes_chain((IPipe*)_content, (IPipe*)&_header, (IPipe*)adp);
	}

	/* 
	* ���͵�һ�����ݰ� 
	*/
	res->rc = STM_CONTINUE;
	res->param.epollEvent = IO_EVENT_EPOLLOUT;

	forward();
	return false;
}

bool HTTPResponder::step1(IOAdapter* adp, int ev, stm_result_t* res)
{
	/* ������Ӧ�� */
	size_t wr = adp->pump(0);
	_bytesSent += wr;
	if( 0 == wr )
	{
		/* ������� */
		forward();
		return true;
	}
	else
	{
		/* �������� */
		res->rc = STM_CONTINUE;
		res->param.epollEvent = IO_EVENT_EPOLLOUT;
	}
	return false;
}

bool HTTPResponder::step2(IOAdapter* adp, int ev, stm_result_t* res)
{
	/* ���� */
	res->rc = STM_DONE;
	return false;
}