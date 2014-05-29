#include "stdafx.h"
#include <algorithm>
#include "IOCP.h"

/************************************************************************/
/*                                                                      */
/************************************************************************/
static u_int __stdcall iocp_worker_thread(void* lpParam)
{
	IOCPSelector* selector = (IOCPSelector*)lpParam;
	return selector->loop();
}

IOCPSelector::IOCPSelector()
	:_iocpBroadcast(NULL), _iocpWorker(NULL), _threads(MIN_IOCP_THREADS), _tids(NULL)
{
	/*
	* ����һ����ɶ˿ڹ㲥����
	*/
	if( NULL == (_iocpBroadcast = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, _threads)) )
	{
		assert(0);
	}

	_threads = GetSystemProcessorNumber();
	if(_threads < MIN_IOCP_THREADS) _threads = MIN_IOCP_THREADS;

	/*
	* ����һ����ɶ˿ڹ�������
	*/
	if( NULL == (_iocpWorker = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, _threads)) )
	{
		assert(0);
	}

	/*
	* ����һ�����ʴ�С��ͬ�����ط�������е�IOCPAdapter
	*/
	_lockPool.init(_threads * 2);

	/*
	* �������ɸ��߳�ִ�� GetQueuedCompletionStatus ����
	*/
	_tids = new uintptr_t[_threads];
	for(int i = 0; i < _threads; ++i)
	{
		if(0 == (_tids[i] = _beginthreadex(NULL, 0, iocp_worker_thread, this, 0, NULL)))
		{
			assert(0);
		}
	}
}

IOCPSelector::~IOCPSelector()
{
	/*
	* ֹͣ�Ĵ���������Ҫ
	*/
	// ֹͣ�����߳�
	if(_tids)
	{
		for(int i = 0; i < _threads; ++i)
		{
			PostQueuedCompletionStatus(_iocpWorker, 0, NULL, NULL);
		}
		for(int i = 0; i < _threads; ++i)
		{
			if( _tids[i] ) 
			{
				WaitForSingleObject(reinterpret_cast<HANDLE>(_tids[i]), INFINITE);
				CloseHandle(reinterpret_cast<HANDLE>(_tids[i])); 
			}
		}
		delete []_tids;
		_tids = NULL;
		_threads = 0;
	}

	// �ر� iocp ���
	if(NULL != _iocpWorker)
	{
		CloseHandle(_iocpWorker);
		_iocpWorker = NULL;
	}

	// ����1:�����ź�������ʱ���е����� epoll �������߳��л����˳��ȴ�״̬���õ�poll����ֵ POLL_EXIT
	//ReleaseSemaphore(_activeAdpaterListSem, 10000000, NULL);

	// ����2: ֱ�ӹرվ��, epoll ����ֵ POLL_ERROR --> �ǲ��ǹ��ڴֱ� -_-!
	if(_iocpBroadcast)
	{
		CloseHandle(_iocpBroadcast);
		_iocpBroadcast = NULL;
	}

	// ɾ������iocpcontext
	for(auto itr = _ctxList.begin(); itr != _ctxList.end(); ++itr)
	{
		IOCPContext* ctx = *itr;
		assert(ctx->adapter == NULL);
		delete ctx;
	}
	_ctxList.clear();

	// ��������
	_lockPool.destroy();
}

void IOCPSelector::lock()
{
	_lock.lock();
}

void IOCPSelector::unlock()
{
	_lock.unlock();
}

Lock* IOCPSelector::allocLock()
{
	return _lockPool.allocate();
}

void IOCPSelector::freeLock(Lock* l)
{
	_lockPool.recycle(l);
}

/*
* �ֶ�����һ���¼�
*/
int IOCPSelector::raiseEvent(IOCPContext* ctx, u_int ev)
{
	ev &= ctx->epollEvent;
	if(ev != IO_EVENT_NONE)
	{
		SET_BIT(ctx->curEvent, ev);
		if(ctx->isInQueue)
		{
		}
		else
		{
			ctx->isInQueue = true;
			PostQueuedCompletionStatus(_iocpBroadcast, 0, (ULONG_PTR)ctx, NULL);
		}
	}
	return 0;
}

/*
* IOCP����Ĺ����̺߳���,����GetQueuedCompletionStatus��ϵͳ������
*/
int IOCPSelector::loop()
{
	int ret = 0;
	for(;;)
	{
		DWORD transfered = 0;
		IOCPContext* ctx = NULL;
		IOCPOVERLAPPED* iocpOlpPtr = NULL;
		if(!GetQueuedCompletionStatus(_iocpWorker, &transfered, reinterpret_cast<PULONG_PTR>(&ctx), (LPOVERLAPPED*)&iocpOlpPtr, INFINITE))
		{
			if(iocpOlpPtr)
			{
				/*
				* IO���������Ϊʧ��
				*/
				onIoCompleted(ctx, false, iocpOlpPtr, transfered);
			}
			else
			{
				/*
				* IOCP��������һЩ����,�����ǳ�ʱ GetLastError returns WAIT_TIMEOUT ��������ϵͳ����
				*/
				assert(0);
				ret = GetLastError();
				break;
			}
		}
		else
		{
		
			if(transfered == 0 && iocpOlpPtr == NULL && ctx == NULL)
			{
				/*
				* Լ���������˳���־
				*/
				break;
			}
			else
			{
				/*
				* ����MSDN��˵��GetQueuedCompletionStatus()����TRUE[ֻ]��ʾ��IOCP�Ķ�����ȡ��һ���ɹ����IO�����İ�.
				* ����"�ɹ�"������ֻ��ָ���������������ɹ����,������ɵĽ���ǲ��ǳ�����Ϊ��"�ɹ�",��һ��.
				* 
				* 1. AcceptEx �� ConnectEx �ɹ��Ļ�,�����Ҫ��һ����/��������(��ַ�����ݳ���),��ô transfered == 0����.
				* 2. Send, Recv �������������Ļ��������ȴ���0,��transfered == 0Ӧ���ж�Ϊʧ��.
				*
				* ʵ�ʲ��Է��ֽ��ܿͻ�������,ִ��һ��Recv����,Ȼ��ͻ������϶Ͽ�,�������е�����,���� Recv transfered == 0����.
				* �ܶ���֮,�ϲ�Ӧ���ж�������������(������AcceptEx��ConnectEx���յ�Զ�̵�ַ,��ר��ָ���ݲ���)���������ȴ���0,
				* �����صĽ����ʾ transfered = 0 ˵������ʧ��.
				*
				* ����ģ�鱾���޷����� transfered�Ƿ����0���жϲ����Ƿ�ɹ�,��Ϊ�ϲ���ȫ����Ͷ��һ������������Ϊ0��Recv����.
				* ���ڷ������������ǳ��õļ���,������Լ�ڴ�.
				*
				*/
				onIoCompleted(ctx, true, iocpOlpPtr, transfered);
			}
		}
	}
	return ret;
}

/*
* IO�������
*
*/
void IOCPSelector::onIoCompleted(IOCPContext* ctx, bool oppResult, IOCPOVERLAPPED* olp, size_t bytesTransfered)
{
	bool canDel = false;
	u_int ev = IO_EVENT_NONE;

	/* ��IOCPAdapter�����������֤ɾ��������ͬ��,Ҫ�ǳ�С��,ע����� IOCPAdapter����������ʱ��û��Ҳ�õ�������������. */
	ctx->lock();

	ev = ctx->onIoCompleted(oppResult, olp, bytesTransfered);

	if(TEST_BIT(ev, IO_EVENT_NONE))
	{
		/* ���� */
	}
	else if(TEST_BIT(ev, IO_EVENT_AUTODELETE))
	{
		/* �Զ�ɾ�� */
		canDel = closeContext(ctx);
	}
	else
	{
	}
	
	/*
	* �����Ծ����(���͵� IOCP:B)
	* ����Ѿ��ڶ�����,����¼�����,��������µ���.
	*/
	if(IO_EVENT_NONE != ev)
	{
		raiseEvent(ctx, ev);
	}

	ctx->unlock();

	// �Զ�ɾ��
	if(canDel)
	{
		freeContext(ctx);
	}
}

IOCPContext* IOCPSelector::allocContext(IOCPAdapter* adp)
{
	IOCPContext* ctx = new IOCPContext(adp->getSock(), adp->getSockType(), IO_RECV_BUF_LEN, IO_SEND_BUF_LEN);
	ctx->setLock(allocLock());
	adp->attach(ctx);
	ctx->adapter = adp;

	lock();
	_ctxList.push_back(ctx);
	unlock();
	return ctx;
}

void IOCPSelector::freeContext(IOCPContext* ctx)
{
	ctx->detach();
	freeLock(ctx->setLock(NULL));

	lock();
	_ctxList.remove(ctx);
	unlock();

	delete ctx;
}

bool IOCPSelector::closeContext(IOCPContext* ctx)
{
	bool canDel = false;
	if(ctx->busy())
	{
		// ����һ���Զ�ɾ���ı��
		ctx->isAutoDelete = true;
	}
	else
	{
		if(ctx->isInQueue)
		{
			// �Ѿ����͵� IOCP:B,�ɹ㲥��ִ��ɾ������.
		}
		else
		{
			canDel = true;
		}
	}

	return canDel;
}

int IOCPSelector::ctl(IOAdapter* adp, int op, u_int ev)
{
	IOCPAdapter *iocpAdp = dynamic_cast<IOCPAdapter*>(adp);
	IOCPContext* ctx = iocpAdp->getContext();
	u_int curEv = IO_EVENT_NONE;

	if(IO_CTL_ADD == op)
	{
		/*
		* ����һ���µ� IOCPContext ָ��� IOCPAdapter
		*/
		if(ctx != NULL)
		{
			return ctl(adp, IO_CTL_MOD, ev);
		}

		ctx = allocContext(iocpAdp);
		if( _iocpWorker != CreateIoCompletionPort((HANDLE)iocpAdp->getSock(), _iocpWorker, (ULONG_PTR)ctx, 0))
		{
			assert(0);
		}
		else
		{
			ctx->epollEvent = ev;
		}
		curEv = ctx->updateEvent(ctx->epollEvent);
	}
	else if(IO_CTL_MOD == op)
	{
		assert(ctx);
		/*
		* �޸��¼�������
		*/
		ctx->lock();

		ctx->epollEvent = ev;
		curEv = ctx->updateEvent(ctx->epollEvent);

		ctx->unlock();
	}
	else if(IO_CTL_DEL == op)
	{
		/* 
		* Windows IOCP �޷�ȡ���׽��ֺ�IOCP����Ĺ���,����, DEL �޷���ȫʵ��. 
		* �� IOAdpater ��������, ֻ����һ�� IO_CTL_DEL. ���ú�Ӧ�þ���ɾ��. DEL ������ ADD ���ܱ�֤������ȷ.
		*/
		/* ���ָ�����Ч�� */
		bool validPtr = true;
		lock();
		if(_ctxList.end() == std::find(_ctxList.begin(), _ctxList.end(), ctx))
		{
			validPtr = false;
		}
		unlock();
		if(validPtr)
		{
			/*
			* ���� IOCPAdapter �� IOCPContext ָ��
			*/
			bool del = false;

			ctx->lock();

			iocpAdp->detach();
			ctx->detach();
			ctx->epollEvent = IO_EVENT_NONE;
			ctx->adapter = NULL;
			del = closeContext(ctx);

			ctx->unlock();

			if(del)
			{
				freeContext(ctx);
			}
		}
		else
		{
			return IO_ERROR_UNDEFINED;
		}
	}
	else
	{
		assert(0);
	}

	// �ֶ������Ѿ����ڵ��¼�
	if(curEv != IO_EVENT_NONE)
	{
		raiseEvent(ctx, curEv);
	}
	return IO_ERROR_SUCESS;
}

int IOCPSelector::wakeup(size_t count)
{
	if(_iocpBroadcast)
	{
		for(size_t i = 0; i < count; ++i)
		{
			PostQueuedCompletionStatus(_iocpBroadcast, 0, NULL, NULL);
		}
	}
	else
	{
		return IO_ERROR_UNDEFINED;
	}

	return IO_ERROR_SUCESS;
}

/*
* select ����һ����ԾIOAdapter(�ɶ�/��д/����),״̬��IOAdapter����,���߷���NULL��ʾ��ʱ�����˳�.
*/
int IOCPSelector::wait(IOAdapter** adp, u_int* ev, u_int timeo /* = INFINITE */)
{
	int ret = IO_WAIT_SUCESS;
	for(;;)
	{
		DWORD transfered = 0;
		IOCPContext *ctx = NULL;
		IOCPOVERLAPPED* iocpOlpPtr = NULL;
		bool canDel = false;
		bool skip = false;
		if(!GetQueuedCompletionStatus(_iocpBroadcast, &transfered, reinterpret_cast<PULONG_PTR>(&ctx), (LPOVERLAPPED*)&iocpOlpPtr, timeo))
		{
			/* �㲥������ر����쳣�˳���� */
			WAIT_TIMEOUT == GetLastError() ? ret = IO_WAIT_TIMEOUT : ret = IO_WAIT_ERROR;
		}
		else
		{
		
			if(transfered == 0 && iocpOlpPtr == NULL && ctx == NULL)
			{
				/*
				* Լ���������˳���־
				*/
				ret = IO_WAIT_EXIT;
			}
			else
			{
				/*
				* ��ȡ���㲥�Ľ��(�������˵�,���͸��û���Ľ��)
				*/
				ctx->lock();

				// ������
				ctx->isInQueue = false;

				// ����¼�������
				if(TEST_BIT(ctx->epollEvent, IO_EVENT_EPOLLONESHOT))
				{
					ctx->epollEvent = IO_EVENT_NONE;
				}

				// ����Ѿ��������Զ�ɾ�������ִ��һ��ɾ������
				if(ctx->isAutoDelete)
				{
					skip = true;
					canDel = closeContext(ctx);
				}
				else
				{
					*adp = ctx->adapter;
					*ev = ctx->curEvent;
					ctx->curEvent = IO_EVENT_NONE;
				}
				ctx->unlock();

				if(canDel)
				{
					freeContext(ctx);
				}
				if(skip)
				{
					continue;
				}
			}
		}
		break;
	}
	return ret;
}

IOSelector* IO_create_selector()
{
	return  new IOCPSelector();
}

int IO_destroy_slector(IOSelector* s)
{
	delete dynamic_cast<IOCPSelector*>(s);
	return 0;
}