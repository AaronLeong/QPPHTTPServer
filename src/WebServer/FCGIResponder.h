/* Copyright (C) 2012 ������
 *
 * ����һ����Դ������,���������ɵ��޸ĺͷ���.
 * ��ֹ������ҵ��;.
 *
 * ��ϵԭ����: querw@sina.com 
*/

#pragma once
#include <list>
#include "HTTPLib.h"
#include "HTTPResponseHeader.h"
#include "Pipes/BufferPipe.h"
#include "Pipes/FilePipe.h"
#include "FCGITransformer.h"
#include "ChunkTransformer.h"

/* 
* FCGI Э��� Responder, FCGIResponder ���� HTTPRequest Ϊ����,���ƺ�FCGI���������ͨ��,������Ӧ��ת���� HTTP �ͻ���
*
* 2014-3-19
* V0.2��FCGIResponder��������д��������ĵĴ���,�߼����ҽṹ����,���ŵ����������������ı���͸��ְ��ص�����,��ʵ֤�������Ĵ����Ȼ��������Ĵ���.
* V0.3��Ҫ�Ƶ�����,�򵥵�,���ŵĴ�������ҵ�׷��.
*/
class FCGIRecord;
class FCGIResponder : public IResponder
{
protected:
	int _svrCode;
	IRequest *_request;
	IHTTPServer *_server;
	IOAdapter* _fcgiAdp;
	u_short _fcgiId;
	IOAdapter* _httpAdp;

	// ͳ����Ϣ
	__int64 _bytesHTTPSent;
	__int64 _bytesHTTPRecv;
	__int64 _bytesFCGISent;
	__int64 _bytesFCGIRecv;

	// �Ƿ񻺴�������ٷ��͵�HTTP�ͻ���
	bool _cacheAll;
	HTTPResponseHeader _header;
	bool _headerFormatted;
	bool _fcgiRequestEnd;

	/*
	* ����ܵ�
	*/
	// 1. ���͵�FCGI������: POST data -> FCGITransformer -> BufferPipe -> FCGI Adapter
	// 2. ��FCGI����������: FCGI Adapter -> BufferPipe
	// _recordPipe �ظ�ʹ��(����ʱ��Ϊ���뻺����,���ʱ��Ϊ���������
	BufferPipe* _recordPipe;
	FCGITransformer* _recordTransformer;

	// 3���͵�HTTP�ͻ��� FCGIResponder -> FilePipe -> BufferPipe -> ChunkFilter -> HTTPResponderHeader -> HTTP Adapter
	bool _httpClientRuning;
	BufferPipe* _cacheBufferPipe;
	FilePipe* _cacheFilePipe;
	ChunkTransformer* _chunkTransformer;

	// ͬ����,���������,������FCGI������ -> FCGIResponder �� FCGIResponder -> HTTP Client ֮����Ҫ��ͬ��
	Lock* _lock;
	void lock();
	void unlock();

	// ׼�� HTTP -> FCGI 
	void prepareFCGIInput();

	// ���� FCGI ������������������
	void disposeRecord(FCGIRecord* record);

	// �������� HTTP client pump
	void tryHttpClientPump();

	bool beforeStep(IOAdapter* adp, int ev, stm_result_t* res);
	bool step0(IOAdapter* adp, int ev, stm_result_t* res);
	bool step1(IOAdapter* adp, int ev, stm_result_t* res);
	bool step2(IOAdapter* adp, int ev, stm_result_t* res);
	bool step3(IOAdapter* adp, int ev, stm_result_t* res);
	bool step4(IOAdapter* adp, int ev, stm_result_t* res);
	
public:
	FCGIResponder(IHTTPServer *server, IRequest* request, bool cacheAll);
	~FCGIResponder();

	// ר�к���,��Ҫ֪���ĸ���FCGI����������
	IOAdapter* setFCGIConnection(u_short fcgiId, IOAdapter* adp);

	// IResponder
	IRequest* getRequest();
	std::string getHeader();
	int getServerCode();
	void statistics(__int64* bytesRecved, __int64* bytesSent);
};

