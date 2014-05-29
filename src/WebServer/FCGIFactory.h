/* Copyright (C) 2011 ������
 *
 * ����һ����Դ������,���������ɵ��޸ĺͷ���.
 * ��ֹ������ҵ��;.
 *
 * ��ϵԭ����: querw@sina.com 
*/

#pragma once
#include "HTTPLib.h"
#include "FCGIRecord.h"

/*
* ����Fast CGI ������ģʽ
* 1. ����ģʽ,��Windows��ʹ��NT�� �����ܵ�.
* 2. Զ��ģʽ,ʹ���׽���.

* FastCGI on NT will set the listener pipe HANDLE in the stdin of
* the new process.  The fact that there is a stdin and NULL handles
* for stdout and stderr tells the FastCGI process that this is a
* FastCGI process and not a CGI process.
*/

/*
* Fast CGI ���ӹ���,�������� FCGI ����,�������͹�����ģʽ�µ� FCGI ����������.
* 
* ˵��: ΪʲôҪʹ�ö����Ĺܵ���.
* ʹ�ö����Ĺܵ����ĺô���ÿ�����Ӷ�Ӧһ�� FCGI ����,��˾Ϳ��԰�ȫ�Ĺر� FCGI ���̶����õ���Ӱ�쵽��������.
*/

/*
* ����FCGI���̶���
*/

class FCGIResponder;
class FCGIFactory : public IResponderFactory, public IOStateMachine
{
public:
	typedef struct
	{
		PROCESS_INFORMATION processInfo; /* ���̾�� */
		char pipeName[MAX_PATH]; /* �ý��̶�Ӧ�Ĺܵ��� */
		uintptr_t thread; /* �������̲����ӹܵ��ķ����߳� */
		IOAdapter* adp;
		FCGIFactory* instPtr;
	}fcgi_process_context_t;
	typedef std::list<fcgi_process_context_t*> fcgi_process_list_t;
private:
	fcgi_server_ctx_t _fcgiServerCtx;
	u_short _fcgiId;
	IHTTPServer* _svr;
	IOStateMachineScheduler* _scheduler;
	Lock _lock;

	fcgi_process_list_t _processList;		// ����FCGI�����б�
	std::list<IOAdapter*> _busyAdpList;		// æµ�����б�
	std::list<IOAdapter*> _idleAdpList;		// �����б�
	std::list<FCGIResponder*> _waitingList;

	void lock();
	void unlock();

	u_short getFCGIId();

	// ����FCGI����
	void dispatch();

	// IOStateMachine
	bool step0(IOAdapter* adp, int ev, stm_result_t* res);

	// stm handler
	void handleResponder(FCGIResponder* responder, IOAdapter* adp, IOStateMachine* sm, stm_result_t* res);

public:
	FCGIFactory(IHTTPServer *svr, IOStateMachineScheduler* scheduler);
	~FCGIFactory();

	// ��������FCGI���̵��̺߳���
	int spawn(fcgi_process_context_t *context);

	void init(fcgi_server_ctx_t* ctx);
	void release();

	// IResponderFactory
	IResponder* catchRequest(IRequest* request);
	void releaseResponder(IResponder* responder);
};

