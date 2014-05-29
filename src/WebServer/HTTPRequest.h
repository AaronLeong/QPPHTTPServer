/* Copyright (C) 2011 ������
 *
 * ����һ����Դ������,���������ɵ��޸ĺͷ���.
 * ��ֹ������ҵ��;.
 *
 * ��ϵԭ����: querw@sina.com 
*/

#pragma once

#include "HTTPLib.h"
#include "Pipes/BufferPipe.h"
#include "Pipes/FilePipe.h"
/*
* HTTP Э���е�"����"���ĵķ�װ
*
* HTTPRequest ����ʵ���� IRequest �ӿ�,��ʼ���к�ӿͻ��˶�ȡ���� HTTP������,��������ͷ�� POST ����
* 
*/

class HTTPRequest : public IRequest
{
protected:
	BufferPipe* _cachePipe;
	BufferPipe* _httpHeader;
	IPipe* _httpPostData;
	IHTTPServer *_server;
	size_t _contentLength;
	__int64 _bytesRecved;
	__int64 _bytesSent;

	void getHeaderBuffer(const char** buf, size_t *len);
	bool beforeStep(IOAdapter* adp, int ev, stm_result_t* res);
	bool step0(IOAdapter* adp, int ev, stm_result_t* res);
	bool step1(IOAdapter* adp, int ev, stm_result_t* res);
	bool step2(IOAdapter* adp, int ev, stm_result_t* res);
	bool step3(IOAdapter* adp, int ev, stm_result_t* res);

public:
	HTTPRequest(IHTTPServer *server);
	virtual ~HTTPRequest();

	/* IRequest */
	HTTP_METHOD method(); // ����HTTP ����
	std::string uri(bool decode); // ���ؿͻ�������Ķ���(�Ѿ�����UTF8����,���Է��ؿ��ַ���)
	std::string field(const char* key); // ��������ͷ�е�һ���ֶ�(HTTPͷ��ֻ��ANSI�ַ�,���Է���string).
	bool range(__int64 &from, __int64 &to);
	bool keepAlive();
	size_t contentLength(); /* ����ͷ�е� Content-Length �ֶε�ֵ */
	std::string getHeader();
	bool isValid();
	IPipe* getPostData();
	void nextRequest();
	void statistics(__int64* bytesRecved, __int64* bytesSent);
};
