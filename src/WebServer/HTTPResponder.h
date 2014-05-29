/* Copyright (C) 2012 ������
 *
 * ����һ����Դ������,���������ɵ��޸ĺͷ���.
 * ��ֹ������ҵ��;.
 *
 * ��ϵԭ����: querw@sina.com 
*/

#pragma once
#include "HTTPLib.h"
#include "HTTPResponseHeader.h"
#include "HTTPRequest.h"

/*
* HTTPResponder ���ǽӿ� IResponder ��ʵ��,��װ�� HTTP Э�����Ӧ����.
* HTTPResponder ���� IRequest HTTP ������Ϊ����,���ɶ�Ӧ����Ӧ����,�������ݷ��͵� HTTP �ͻ���.
* 
*/

class HTTPContent;
class HTTPResponder : public IResponder
{
protected:
	HTTPResponseHeader _header;
	IRequest *_request;
	IHTTPServer *_server;
	HTTPContent* _content;
	__int64 _bytesRecved;
	__int64 _bytesSent;
	int _svrCode;

	bool makeResponseHeader(int svrCode);	
	void getHeaderBuffer(const char** buf, size_t *len);
	bool beforeStep(IOAdapter* adp, int ev, stm_result_t* res);
	bool step0(IOAdapter* adp, int ev, stm_result_t* res);
	bool step1(IOAdapter* adp, int ev, stm_result_t* res);
	bool step2(IOAdapter* adp, int ev, stm_result_t* res);

public:
	HTTPResponder(IHTTPServer *server, IRequest *req);
	HTTPResponder(IHTTPServer *server, IRequest *req, int sc, const char* msg);
	virtual ~HTTPResponder();

	IRequest* getRequest();
	std::string getHeader();
	int getServerCode();
	void statistics(__int64* bytesRecved, __int64* bytesSent);
};