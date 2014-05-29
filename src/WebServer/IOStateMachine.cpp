#include "StdAfx.h"
#include <functional>
#include "Lock.h"
#include "IOStateMachine.h"

IOStateMachine::IOStateMachine()
	: _state(0)
{
}

IOStateMachine::~IOStateMachine()
{
}

int IOStateMachine::current()
{
	return _state;
}

int IOStateMachine::forward(size_t steps)
{
	return _state += steps;
}

int IOStateMachine::backward(size_t steps)
{
	return _state -= steps;
}

void IOStateMachine::run(IOAdapter* adp, int ev, stm_result_t* res)
{
	bool c = true;
	while(c)
	{
		/* ��¼״̬����� */
		res->st = _state;

		/* Ԥ���� */
		c = beforeStep(adp, ev, res);

		if(c)
		{

			/* ִ�ж�Ӧ��״̬������ */
			switch(_state)
			{
			case 0:
				{
					c = step0(adp, ev, res);
				}
				break;
			case 1:
				{
					c = step1(adp, ev, res);
				}
				break;
			case 2:
				{
					c = step2(adp, ev, res);
				}
				break;
			case 3:
				{
					c = step3(adp, ev, res);
				}
				break;
			case 4:
				{
					c = step4(adp, ev, res);
				}
				break;
			case 5:
				{
					c = step5(adp, ev, res);
				}
				break;
			case 6:
				{
					c = step6(adp, ev, res);
				}
				break;
			case 7:
				{
					c = step7(adp, ev, res);
				}
				break;
			case 8:
				{
					c = step8(adp, ev, res);
				}
				break;
			case 9:
				{
					c = step9(adp, ev, res);
				}
				break;
			default:
				assert(0);
				c = false;
			}

			/* ���� */
			if(c)
			{
				c = afterStep(adp, ev, res);
			}
		}
	}
}

bool IOStateMachine::beforeStep(IOAdapter* adp, int ev, stm_result_t* res)
{
	return true;
}

bool IOStateMachine::afterStep(IOAdapter* adp, int ev, stm_result_t* res)
{
	return true;
}

bool IOStateMachine::step0(IOAdapter* adp, int ev, stm_result_t* res)
{
	assert(0);
	return false;
}

bool IOStateMachine::step1(IOAdapter* adp, int ev, stm_result_t* res)
{
	assert(0);
	return false;
}

bool IOStateMachine::step2(IOAdapter* adp, int ev, stm_result_t* res)
{
	assert(0);
	return false;
}

bool IOStateMachine::step3(IOAdapter* adp, int ev, stm_result_t* res)
{
	assert(0);
	return false;
}

bool IOStateMachine::step4(IOAdapter* adp, int ev, stm_result_t* res)
{
	assert(0);
	return false;
}

bool IOStateMachine::step5(IOAdapter* adp, int ev, stm_result_t* res)
{
	assert(0);
	return false;
}

bool IOStateMachine::step6(IOAdapter* adp, int ev, stm_result_t* res)
{
	assert(0);
	return false;
}

bool IOStateMachine::step7(IOAdapter* adp, int ev, stm_result_t* res)
{
	assert(0);
	return false;
}

bool IOStateMachine::step8(IOAdapter* adp, int ev, stm_result_t* res)
{
	assert(0);
	return false;
}

bool IOStateMachine::step9(IOAdapter* adp, int ev, stm_result_t* res)
{
	assert(0);
	return false;
}

/************************************************************************/
/* ������                                                               */
/************************************************************************/
class IOStateMachineSchedulerImpl : public IOStateMachineScheduler
{
private:
	typedef struct iostatemachine_context
	{
		IOStateMachine* sm;
		sm_handler_t handlerOnStepDone;
		sm_handler_t handlerOnDone;
		sm_handler_t handlerOnAbort;
	}iosm_context_t;
	typedef std::list<iosm_context_t*> iosmcontext_list_t;
	IOSelector* _ioSelector;
	Lock _lock;
	iosmcontext_list_t _iosmctxList;

	int _threads;
	uintptr_t* _tids;

	int execute(IOAdapter* adp, iosm_context_t* ctx, unsigned int ev);
	
public:
	IOStateMachineSchedulerImpl();
	~IOStateMachineSchedulerImpl();

	int install(IOAdapter* adp, unsigned int ev, IOStateMachine* sm, sm_handler_t onStepDone, sm_handler_t onDone, sm_handler_t onAbort);
	int reinstall(IOAdapter* adp, unsigned int ev, IOStateMachine* sm, sm_handler_t onStepDone, sm_handler_t onDone, sm_handler_t onAbort);
	int uninstall(IOAdapter* adp);
	int schedule();

	int start(size_t threads);
	int stop();
};

IOStateMachineSchedulerImpl::IOStateMachineSchedulerImpl()
	: _ioSelector(NULL), _threads(0), _tids(NULL)
{
	_ioSelector = IO_create_selector();
	assert(_ioSelector);
}

IOStateMachineSchedulerImpl::~IOStateMachineSchedulerImpl()
{
	// ֹͣ����
	stop();

	// ����IO������/ѡ����
	IO_destroy_slector(_ioSelector);

	// ɾ���Ѿ���װ��״̬��
	assert(_iosmctxList.size() == 0);
	for(auto itr = _iosmctxList.begin(); itr != _iosmctxList.end(); ++itr)
	{
		delete *itr;
	}
}

int IOStateMachineSchedulerImpl::execute(IOAdapter* adp, IOStateMachineSchedulerImpl::iosm_context_t* ctx, unsigned int ev)
{
	stm_result_t res;
	ctx->sm->run(adp, ev, &res);

	// ���ദ�����н��
	if(STM_CONTINUE == res.rc)
	{
		// ������� ONESHOT ��־��ֹ��������
		SET_BIT(res.param.epollEvent, IO_EVENT_EPOLLONESHOT | IO_EVENT_EPOLLERR | IO_EVENT_EPOLLHUP);
		_ioSelector->ctl(adp, IO_CTL_MOD, res.param.epollEvent);
	}
	else if(STM_STEPDONE == res.rc)
	{
		if(ctx->handlerOnStepDone)
		{
			ctx->handlerOnStepDone(adp, ctx->sm, &res);
		}

		SET_BIT(res.param.epollEvent, IO_EVENT_EPOLLONESHOT | IO_EVENT_EPOLLERR | IO_EVENT_EPOLLHUP);
		_ioSelector->ctl(adp, IO_CTL_MOD, res.param.epollEvent);
	}
	else if(STM_ABORT == res.rc)
	{
		// ��ֹ(�쳣�˳�)
		if(ctx->handlerOnAbort)
		{
			ctx->handlerOnAbort(adp, ctx->sm, &res);
		}

		// ж��
		//uninstall(ctx->adp);
	}
	else if(STM_DONE == res.rc)
	{
		// ���(�����˳�)
		if(ctx->handlerOnDone)
		{
			ctx->handlerOnDone(adp, ctx->sm, &res);
		}

		// ж��
		//uninstall(ctx->adp);
	}
	else if(STM_SLEEP == res.rc)
	{
		// ��ʱ����,����һ����ʱ��
	}
	else if(STM_PAUSE == res.rc)
	{
		// ��ͣ
	}
	else if(STM_RESUME == res.rc)
	{
		// ��������
	}
	else
	{
		assert(0);
	}

	return res.rc;
}

int IOStateMachineSchedulerImpl::install(IOAdapter* adp, unsigned int ev, IOStateMachine* sm, sm_handler_t onStepDone, sm_handler_t onDone, sm_handler_t onAbort)
{
	iosm_context_t* ctx = new iosm_context_t;
	ctx->sm = sm;
	ctx->handlerOnStepDone = onStepDone;
	ctx->handlerOnDone = onDone;
	ctx->handlerOnAbort = onAbort;
	adp->setUserData(ctx);

	_lock.lock();
	_iosmctxList.push_back(ctx);
	_lock.unlock();

	/* ���õ�һ�ε��ȵ����� */
	SET_BIT(ev, IO_EVENT_EPOLLONESHOT | IO_EVENT_EPOLLERR | IO_EVENT_EPOLLHUP);
	return _ioSelector->ctl(adp, IO_CTL_ADD, ev);
}

int IOStateMachineSchedulerImpl::reinstall(IOAdapter* adp, unsigned int ev, IOStateMachine* sm, sm_handler_t onStepDone, sm_handler_t onDone, sm_handler_t onAbort)
{
	iosm_context_t* ctx = (iosm_context_t*)adp->getUserData();
	assert(ctx);
	
	ctx->sm = sm;
	ctx->handlerOnStepDone = onStepDone;
	ctx->handlerOnDone = onDone;
	ctx->handlerOnAbort = onAbort;

	/* ���õ�һ�ε��ȵ����� */
	SET_BIT(ev, IO_EVENT_EPOLLONESHOT | IO_EVENT_EPOLLERR | IO_EVENT_EPOLLHUP);
	return _ioSelector->ctl(adp, IO_CTL_MOD, ev);
}

int IOStateMachineSchedulerImpl::uninstall(IOAdapter* adp)
{
	iosm_context_t* ctx = (iosm_context_t*)adp->getUserData();
	
	_ioSelector->ctl(adp, IO_CTL_DEL, IO_EVENT_NONE);

	_lock.lock();
	_iosmctxList.remove(ctx);
	_lock.unlock();

	delete ctx;

	adp->setUserData(NULL);
	return 0;
}

int IOStateMachineSchedulerImpl::schedule()
{
	int ret = 0;
	for(;;)
	{
		IOAdapter *adp = NULL;
		unsigned int ev = IO_EVENT_NONE;
		ret = _ioSelector->wait(&adp, &ev);
		
	
		if(IO_WAIT_SUCESS == ret)
		{
			/*
			* ��ȡadp ��Ӧ�� exec unit ������֮
			*/
			iosm_context_t* unit = (iosm_context_t*)adp->getUserData();
			execute(adp, unit, ev);
		}
		else if(IO_WAIT_EXIT == ret)
		{
			
			/*
			* �˳�
			*/
			break;
		}
		else
		{
			// EPOLL_TIMEOUT
			assert(0);
			break;
		}
	}

	return ret;
}

static unsigned int __stdcall schedule_loop(void* lpParam)
{
	IOStateMachineSchedulerImpl* schd = (IOStateMachineSchedulerImpl*)lpParam;
	return schd->schedule();
}

int IOStateMachineSchedulerImpl::start(size_t threads)
{
	assert(threads > 0);
	if(threads == 0) threads = 2;
	_threads = threads;

	// ���������߳�ִ�е��Ⱥ�����ʼ�����Ѿ���װ��״̬��
	_tids = new uintptr_t[_threads];
	for(int i = 0; i < _threads; ++i)
	{
		if(0 == (_tids[i] = _beginthreadex(NULL, 0, schedule_loop, this, 0, NULL)))
		{
			assert(0);
		}
	}

	return 0;

}

int IOStateMachineSchedulerImpl::stop()
{
	// ����
	if(_ioSelector)
	{
		_ioSelector->wakeup(_threads);
	}

	// �ȴ������߳��˳�
	if(_tids)
	{
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
	}

	return 0;
}

IOStateMachineScheduler* create_scheduler()
{
	return new IOStateMachineSchedulerImpl();
}

void destroy_scheduler(IOStateMachineScheduler* scheduler)
{
	delete (IOStateMachineSchedulerImpl*)scheduler;
}