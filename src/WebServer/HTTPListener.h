#pragma once

#include "HTTPLib.h"

/*
 * �����׽���ִ�е�Ԫ
 * ͨ���� 80 �� 443 ����������Ԫ
 *
*/
class HTTPListener : public IOStateMachine
{
private:
	IOAdapter* _newAdp;
	bool step0(IOAdapter* adp, int ev, stm_result_t* res);

public:
	HTTPListener();
	virtual ~HTTPListener();
	IOAdapter* getAdapter();
};

