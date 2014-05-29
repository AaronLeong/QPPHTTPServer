#pragma once
#include "HTTPLib.h"
/*
 * HTTP��Ӧ����
 * Ϊ��ͬ�� HTTP���� ���ɶ�Ӧ�� HTTP��Ӧ.
 * DefaultResponderFactory �� FCGIResponderFactory
 *
 */

/* Ĭ�ϵ���Ӧ������,��������һ��� HTTP���� */
class DefaultResponderFactory : public IResponderFactory
{
private:
	IHTTPServer *_svr;
public:
	DefaultResponderFactory(IHTTPServer* svr);
	~DefaultResponderFactory();

	IResponder* catchRequest(IRequest* req);
	void releaseResponder(IResponder* res);
};