#pragma once
#include <functional>
#include <list>
#include "IOSelector.h"

/*
* IOStateMachine ״̬������
* 1. ��������10��״̬
* 2. stepx�ķ���ֵtrue��ʾ����ִ��,false��ʾ�ȴ���һ�ε���.
*
* by Q++ Studio ������ 2014.4.14
*/

/* 
* ʵ��һ:
* 1. ����ά��һ�� std::function ����
* 2. �������� std::bind �Ѹ���״̬��������˳����ӽ�����,�Ϳ���ʵ�������� step ���������� step0 ~ 9.
*

* ʵ�ֶ�:
* Ԥ���� step0 ~ 9 һ��10���麯��,������ֻҪ����ʵ�ּ�������.
* �������ܱ��İ취,�������дȴ�ǳ����,�������ڲ����̶���������.
*/

/* ״̬�� run ����ֵ */
#define STM_ABORT -1		//��ֹ
#define STM_CONTINUE 0		//����ִ�� epoll
#define STM_STEPDONE 1		//һ��stepִ�����
#define STM_DONE 2			//״̬��ִ�����
#define STM_PAUSE 5			//��ͣ״̬��(��ͬһ��״̬������һ��IOAdapter��������,ȡ���ھ������
#define STM_RESUME 6		//��������
#define STM_SLEEP 10		//˯��һ��ʱ����Ѽ�������exec

typedef union stm_result_param
{
	unsigned int sleepTime;
	unsigned int epollEvent;
	unsigned int errorCode;
	IOAdapter*	resumeAdp;
}stm_res_param_t;

typedef struct STM_RESULT
{
	int rc;						// result code STM_XXX
	int st;						// state index
	stm_res_param_t param;		// parameter
}stm_result_t;

/* ״̬������ */
class IOStateMachine
{
private:
	int _state;

protected:
	virtual bool beforeStep(IOAdapter* adp, int ev, stm_result_t* res);
	virtual bool step0(IOAdapter* adp, int ev, stm_result_t* res);
	virtual bool step1(IOAdapter* adp, int ev, stm_result_t* res);
	virtual bool step2(IOAdapter* adp, int ev, stm_result_t* res);
	virtual bool step3(IOAdapter* adp, int ev, stm_result_t* res);
	virtual bool step4(IOAdapter* adp, int ev, stm_result_t* res);
	virtual bool step5(IOAdapter* adp, int ev, stm_result_t* res);
	virtual bool step6(IOAdapter* adp, int ev, stm_result_t* res);
	virtual bool step7(IOAdapter* adp, int ev, stm_result_t* res);
	virtual bool step8(IOAdapter* adp, int ev, stm_result_t* res);
	virtual bool step9(IOAdapter* adp, int ev, stm_result_t* res);
	virtual bool afterStep(IOAdapter* adp, int ev, stm_result_t* res);

	IOStateMachine();
public:
	virtual ~IOStateMachine();
	int current();
	int forward(size_t steps = 1);
	int backward(size_t steps = 1);
	void run(IOAdapter* adp, int ev, stm_result_t* res);
};

/* ״̬��������� */
typedef std::list<std::pair<IOAdapter*, IOStateMachine*> > iosm_list_t;

/*
* IO״̬��������
* 
* TIP: ���ʱ���и�����,��Ȼ״̬�������Զ����,Ϊʲô����Ҫ3����������? ֱ����״̬����������ʵ����Щ���ܲ��Ϳ�������?������ֻ��Ҫ����״̬�����м���.
* ����������ԭ����Ҫ��ɾ��������.����״̬����������ʵ�� onDone, onDone Ҫ������֮һ������ɾ�����״̬��,�ͻ������ɱ������,��Ȼ����ʵ��,���Ǻ����׳���(ɾ����Ͳ����ٷ������Ա,thisָ���Ѿ�ʧЧ)
* �����ʹ�õ���������Ĵ�����ָ��,���������Ա�ָ֤�����Ч��.����о���Ҳ����ȻһЩ.
*/
typedef std::function<void (IOAdapter*, IOStateMachine*, stm_result_t*)> sm_handler_t;
class IOStateMachineScheduler
{
protected:
	virtual ~IOStateMachineScheduler() {};
public:
	// ��װ״̬��,������Ϻ��Զ�ж��.
	virtual int install(IOAdapter* adp, u_int ev, IOStateMachine* sm, sm_handler_t onStepDone, sm_handler_t onDone, sm_handler_t onAbort) = 0;
	virtual int reinstall(IOAdapter* adp, u_int ev, IOStateMachine* sm, sm_handler_t onStepDone, sm_handler_t onDone, sm_handler_t onAbort) = 0;
	virtual int uninstall(IOAdapter* adp) = 0;

	// ��ʼ/ֹͣ����
	virtual int start(size_t threads) = 0;
	virtual int stop() = 0;
};

/* ������ */
extern IOStateMachineScheduler* create_scheduler();
extern void destroy_scheduler(IOStateMachineScheduler* scheduler);
