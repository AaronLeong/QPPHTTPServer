#pragma once

/* Copyright (C) 2011 ������
 *
 * ����һ����Դ������,���������ɵ��޸ĺͷ���.
 * ��ֹ������ҵ��;.
 *
 * ��ϵԭ����: querw@sina.com 
*/

#include <list>
#include <vector>
#include "Lock.h"

/*
* ��������ֵ����
*/
const int TQ_SUCCESS = 0;
const int TQ_ERROR = 1;

/*
* ʹ�� QueryPerformanceFrequency() ʵ�ֵĸ߾��ȼ�ʱ��
*/
class HighResolutionTimer
{
private:
	__int64 _frequency;
public:
	HighResolutionTimer(bool high);
	~HighResolutionTimer();

	__int64 now(); /* ���ص�ǰʱ�� */
	__int64 getCounters(__int64 ms);
	__int64 getMs(__int64 counter);
};

/*
* ��ʱ������
* Ŀ��: ʵ�ֶ�ʱ������.
* 
* һ�������߳�ά��һ����ʱ������,��ʱ��������һ���������(����������Ч��ʱ������),��쳬ʱ�Ķ�ʱ�������ڶ�ͷ.
* ֻ�й����߳̿��Բ�����ʱ������.
*
* ������һ��ר�ŵĲ�������.
* ���/�޸�/ɾ������ʱ,�ڰѲ������ͷ������������,Ȼ���ѹ����߳�.
* �����߳�һ�δ��������в����������ŶӵĲ���.
*
* 2014-3-19 - V0.3
* 1. ȡ����������,ֱ���޸Ķ�ʱ������.
* 2. ����ɾ�����һ����ʱ��ʱ���øö�ʱ����ʱ���ٽ�״̬_curTimerָ��ʧЧBUG.
* 3. changTimer()�������ڿ���.
* 4. ����"��ʱ������״̬"�ĸ�����ƶ�ʱ������,�����߼���Ӧ�ø������һ��.
*/

class TimerQueue
{
public:
	/*
	* ���Ͷ���
	*/
	typedef void* timer_t;
	typedef void (*timer_func_t)(timer_t, void*);

protected:

	typedef struct timer_desc_t	
	{
		timer_func_t funcPtr; /* �ص�������ַ */
		void* param;
		HANDLE ev;	/* �ص�������ɺ��֪ͨ�¼� */
		__int64 expireCounters; /* ��ʱ����ʱ�� */
		int st; /* ��ʱ��״̬: 0 - δִ��; 1 - ����ִ�лص�����; 2 - �����;*/
		bool waitting; /* �Ƿ����߳����ڵȴ��ص�����ִ����� */
		bool autoDelete;
	};
	typedef std::list<timer_desc_t*> timer_list_t;
	
	/*
	* �Զ���TimerQueue
	*/
	HANDLE _waitableTimer; /* ϵͳ��ʱ������ */
	uintptr_t _timerThread; /* ��ʱ�������߳� */
	int _state;	/* ��ʱ�����еĹ���״̬[����TIMER/���ö�ʱ��/�˳�] */

	timer_list_t* _timerList; /* ��ʱ��������� */
	timer_desc_t* _curTimer; 
	Lock* _lock; /* ͬ���� */
	HighResolutionTimer _hrt; /* �߾��ȼ�ʱ�� */

	/* 
	* ��ʱ������� 
	*/
	size_t _poolSize;
	size_t _poolPos;
	timer_desc_t** _timerPool;

	timer_desc_t* allocTimer();
	int freeTimer(timer_desc_t* timerPtr);

	/*
	* �����̺߳���
	*/
	static unsigned int __stdcall workerProc(void* param);

	/* 
	* WaitableTimer ������ʱ�ɹ����̵߳��� 
	*/
	bool proc(HANDLE ce); 

	/*
	* ��������,������
	*/
	int setNextTimer(); /* ���㶨ʱ��������ʱ�� */
	timer_desc_t* getFirstTimer();
	bool isValidTimer(timer_desc_t* timerPtr);
	bool inqueue(timer_desc_t* timerPtr); /* �����Ҫ�������ö�ʱ���򷵻�true,���򷵻�false */
	void wakeup();	/* ���ѹ����߳� */
	void oppAcquire(timer_desc_t* timerPtr, int oppType);

public:
	TimerQueue(size_t poolSize = 128);
	~TimerQueue();

	/*
	* ��ʼ��������
	*/
	int init();
	int destroy();

	/*
	* ����,�޸�,ɾ����ʱ��
	*/
	timer_t createTimer(DWORD timeout, timer_func_t callbackFunc, void* param);
	int changeTimer(timer_t timerPtr, DWORD timeout);
	int deleteTimer(timer_t timerPtr, bool wait);
};