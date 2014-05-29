#include "StdAfx.h"
#include "FCGIFactory.h"
#include "HTTPLib.h"
#include "HTTPResponder.h"
#include "FCGIResponder.h"

static unsigned int WINAPI fcgi_spawn_proc(void* param)
{
	FCGIFactory::fcgi_process_context_t* ctx = (FCGIFactory::fcgi_process_context_t*)param;
	return ctx->instPtr->spawn(ctx);
}

FCGIFactory::FCGIFactory(IHTTPServer *svr, IOStateMachineScheduler* scheduler)
	: _svr(svr), _scheduler(scheduler), _fcgiId(1)
{
	memset(&_fcgiServerCtx, 0, sizeof(fcgi_server_ctx_t));
}

FCGIFactory::~FCGIFactory()
{
	release();
}

void FCGIFactory::lock()
{
	_lock.lock();
}

void FCGIFactory::unlock()
{
	_lock.unlock();
}

u_short FCGIFactory::getFCGIId()
{
	u_short newid = 0;
	lock();
	newid = _fcgiId++;
	unlock();
	return newid;
}

void FCGIFactory::init(fcgi_server_ctx_t* ctx)
{
	memcpy(&_fcgiServerCtx, ctx, sizeof(fcgi_server_ctx_t));
}

void FCGIFactory::release()
{
	/* �ͷ��������� */
	for(auto itr = _busyAdpList.begin(); itr != _busyAdpList.end(); ++itr)
	{
		IO_destroy_adapter(*itr);
	}
	_busyAdpList.clear();

	for(auto itr = _idleAdpList.begin(); itr != _idleAdpList.end(); ++itr)
	{
		IO_destroy_adapter(*itr);
	}
	_idleAdpList.clear();

	/* ��յȴ����� */
	_waitingList.clear();

	/* ɱ��FCGI���� */
	for(auto itr = _processList.begin(); itr != _processList.end(); ++itr)
	{
		fcgi_process_context_t* context = *itr;

		/* ��������߳�����������,�ȴ����� */
		if(context->thread)
		{
			if( WAIT_OBJECT_0 != WaitForSingleObject((HANDLE)context->thread, 0) )
			{
				TerminateThread((HANDLE)context->thread, 1);
			}
			CloseHandle((HANDLE)context->thread);
		}

		/* ���FCGI������������,��ֹ֮ */
		if(context->processInfo.hProcess)
		{
			if(WAIT_OBJECT_0 != WaitForSingleObject(context->processInfo.hProcess, 0))
			{
				TerminateProcess(context->processInfo.hProcess, 1);
			}
			CloseHandle(context->processInfo.hThread);
			CloseHandle(context->processInfo.hProcess);
		}

		delete context;
	}
	_processList.clear();
	_fcgiId = 1;
}

// �ؼ�����,����һ��FCGI����
void FCGIFactory::dispatch()
{
	if(_waitingList.size() <= 0) return;
	IOAdapter* adp = NULL;
	
	// �����ж���
	if(!_idleAdpList.empty())
	{
		// �ӿ��ж����л�ȡһ������,�����뵽æµ������
		adp = _idleAdpList.back();
		_idleAdpList.pop_back();
		_busyAdpList.push_back(adp);
	}
	else
	{
		// �Ƿ�������һ���µ�����
		if(_busyAdpList.size() < _fcgiServerCtx.maxConnections)
		{
			if(_fcgiServerCtx.port != 0)
			{
				// Զ��ģʽ,ֱ�Ӵ���һ��socket IOAdapter
				IOAdapter* newAdp = IO_create_adapter();
				newAdp->bind(NULL, 0);
				newAdp->connect(_fcgiServerCtx.path, _fcgiServerCtx.port);
				_scheduler->install(newAdp, IO_EVENT_EPOLLOUT, this, NULL, NULL, NULL);

				// �´�����δ���������ӽ���æµ����
				_busyAdpList.push_back(newAdp);
			}
			else
			{
				// ����ģʽ����һ���߳�����ɴ����ӽ���,�ȴ������ܵ��Ⱥ�ʱ����
				fcgi_process_context_t *ctx = new fcgi_process_context_t;
				memset(ctx, 0, sizeof(fcgi_process_context_t));
				ctx->instPtr = this;
				strcpy(ctx->pipeName, _fcgiServerCtx.path);
				ctx->thread = _beginthreadex(NULL, 0, fcgi_spawn_proc, ctx, 0, NULL);

				if(0 == ctx->thread)
				{
					LOGGER_CERROR(theLogger, _T("�޷�Ϊ����FCGI���ؽ�������һ�������߳�,_beginthreadex����ʧ��,������:%d.\r\n"), errno);
					delete ctx;
				}
				else
				{
					_processList.push_back(ctx);
				}
			}
		}
		else
		{
			// ʲô������,�ȴ���������
		}
	}

	if(adp)
	{
		// �ӵȴ������л�ȡһ��FCGIResponder
		FCGIResponder* responder = _waitingList.front();
		_waitingList.pop_front();

		// FCGIResponder��Ҫ֪���ĸ������Ǻ�FCGI������֮�������,�ĸ��Ǻ�HTTP�ͻ���֮�������,���д�һ����.
		responder->setFCGIConnection(getFCGIId(), adp);

		// ��װ״̬��
		_scheduler->install(adp, IO_EVENT_EPOLLOUT, responder, NULL, 
			std::bind(&FCGIFactory::handleResponder, this, responder, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3),
			std::bind(&FCGIFactory::handleResponder, this, responder, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	}
}

// FCGIResponder ��FCGI������������ɺ󴥷�.
void FCGIFactory::handleResponder(FCGIResponder* responder, IOAdapter* adp, IOStateMachine* sm, stm_result_t* res)
{
}

IResponder* FCGIFactory::catchRequest(IRequest* request)
{
	// ����uri��������ʽ�������Ƿ񲶻�.
	IResponder* responder = NULL;
	std::string scriptFileName;
	if(_svr->mapServerFilePath(request->uri(true), scriptFileName))
	{
		std::string ext;
		get_file_ext(scriptFileName, ext);

		if(match_file_ext(ext, _fcgiServerCtx.exts))
		{
			// ����һ��FCGIResponder,����ȴ�����,Ȼ�����
			if(_fcgiServerCtx.maxWaitListSize == 0 || _waitingList.size() < _fcgiServerCtx.maxWaitListSize)
			{
				FCGIResponder* fcgiResponder = new FCGIResponder(_svr, request, _fcgiServerCtx.cacheAll == TRUE);
				_waitingList.push_back(fcgiResponder);

				dispatch();
			}
			else
			{
				// �ﵽ���������,����503,������æµ
				responder = new HTTPResponder(_svr, request, SC_SERVERBUSY, g_HTTP_Server_Busy);
			}
		}
	}

	return responder;
}

void FCGIFactory::releaseResponder(IResponder* responder)
{
	// �� typeid ��ô�о���ô������?
	if(typeid(*responder) == typeid(HTTPResponder))
	{
		// �����HTTPResponder,ֱ��ɾ��
	}
	else
	{
		// �����FCGIResponder,����FCGI����,Ȼ��ɾ��.
		FCGIResponder* fcgiResponder = (FCGIResponder*)responder;
		IOAdapter* adp = fcgiResponder->setFCGIConnection(0, NULL);
		_idleAdpList.push_back(adp);
	}
	delete responder;
}

/*
* ��һ���������̴߳���һ��FCGI�����ӽ���,���������ܵ�����
*/
int FCGIFactory::spawn(fcgi_process_context_t* context)
{
	/* pipeName ��������ʱ�ǽ���·�� */
	char fcgProcPath[MAX_PATH];
	strcpy(fcgProcPath, context->pipeName);

	/* ����һ��Ψһ�Ĺܵ��� */
	unsigned int seed = static_cast<unsigned int>(time( NULL )); /* ȷ����ͬexeʵ���䲻ͬ */
	seed += reinterpret_cast<unsigned int>(context); /* ȷ��ͬһ��exeʵ����,��ͬ���̲߳�ͬ */
	srand(seed);
	sprintf(context->pipeName, "%s\\%04d_%04d", FCGI_PIPE_BASENAME, rand() % 10000, rand() % 10000);
	TRACE("pipename:%s.\r\n", context->pipeName);

	/* ����һ�������ܵ� */
	HANDLE hPipe = CreateNamedPipe(AtoT(context->pipeName).c_str(),  PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_BYTE | PIPE_WAIT | PIPE_READMODE_BYTE,
		PIPE_UNLIMITED_INSTANCES,
		4096, 4096, 0, NULL);
	if( INVALID_HANDLE_VALUE == hPipe )
	{
		LOGGER_CERROR(theLogger, _T("�޷����������ܵ�,����FCGI��������ʧ��,������:%d\r\n"), GetLastError());
	}
	if(!SetHandleInformation(hPipe, HANDLE_FLAG_INHERIT, TRUE))
	{
		LOGGER_CERROR(theLogger, _T("SetHandleInformation()����ʧ��,����FCGI��������ʧ��,������:%d\r\n"), GetLastError());
	}

	/* �������ܵ�ΪSTDIN����һ������FCGI���� */
	STARTUPINFO startupInfo;
	memset(&startupInfo, 0, sizeof(STARTUPINFO));
	startupInfo.cb = sizeof(STARTUPINFO);
	startupInfo.dwFlags = STARTF_USESTDHANDLES;
	startupInfo.hStdInput  = hPipe;
	startupInfo.hStdOutput = INVALID_HANDLE_VALUE;
	startupInfo.hStdError  = INVALID_HANDLE_VALUE;

	if( !CreateProcess(AtoT(fcgProcPath).c_str(), NULL, NULL, NULL, TRUE,

#ifdef _DEBUG /* ����״̬��,������PHP-CGI���̴�����̨����, releaseʱ��������̨���� */	
		0, 
#else
		CREATE_NO_WINDOW,
#endif

		NULL, NULL, &startupInfo, &context->processInfo))
	{
		LOGGER_CERROR(theLogger, _T("CreateProcess()����ʧ��,�޷����ɱ���FCGI����,����:%s.\r\n"), AtoT(get_last_error()).c_str());
	}

	/* �ȴ������ܵ� */
	if(!WaitNamedPipe(AtoT(context->pipeName).c_str(), FCGI_CONNECT_TIMEO))
	{
		LOGGER_CERROR(theLogger, _T("���������ܵ�ʧ��[%s]\r\n"), AtoT(get_last_error()).c_str());
	}

	/* ����׼������ */
	context->adp = IO_create_adapter(context->pipeName);
	_scheduler->install(context->adp, IO_EVENT_EPOLLOUT, this, NULL, NULL, NULL);
	return 0;
}

bool FCGIFactory::step0(IOAdapter* adp, int ev, stm_result_t* res)
{
	bool adpReady = true;
	if(TEST_BIT(ev, IO_EVENT_EPOLLERR))
	{
		// ���ش���,�����־
		// ..
		adpReady = false;
	}

	if(TEST_BIT(ev, IO_EVENT_EPOLLHUP))
	{
		// �Ҷ�,�����־
		// ..
		adpReady = false;
	}

	if(!adpReady)
	{
		// �ر�ʧ�ܵ�����
		// ���æµ����Ϊ��,˵������FCGI���Ӷ�ʧ����, ʹ���еȴ��е�FCGIResponder��ʧ�ܷ���,Ȼ����յȴ�����
	}
	else
	{
		// ������׼������,��æµ�������Ƶ����ж���
		// ����һ�����Ӹ��ȴ������е�FCGIResponder
		dispatch();
	}

	
	return false;
}