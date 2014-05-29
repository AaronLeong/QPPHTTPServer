/* Copyright (C) 2011 ������
 *
 * ����һ����Դ������,���������ɵ��޸ĺͷ���.
 * ��ֹ������ҵ��;.
 *
 * ��ϵԭ����: querw@sina.com 
*/

#if !defined(_HTTPFILESERVER_H_)
#define _HTTPFILESERVER_H_

/*
#include "winsock2.h"
#pragma comment(lib, "ws2_32.lib")
*/
#include <functional>
#include "HTTPLib.h"
#include "HTTPListener.h"
/*
* HTTPServer��
* Ŀ��: ����һ��HTTP Server���ڴ��еĴ���.
* 1. ���������׽���.
* 2. ����HTTP���Ӱ��� HTTPRequest, HTTPResponder ��,���������ӽ���ʱ���� HTTPRequest ���󲢵���������ں��� run()
*	 ����������������ӵĴ������.
* 3. �ṩ������ȡ������.
* 
* by ������ - Que's C++ Studio
* 2011.7.7
*/

typedef std::vector<IResponderFactory*> resfactory_list_t;
class HTTPConnection;
class HTTPServer : public IHTTPServer
{
protected:
	IOStateMachineScheduler* _scheduler; /* ״̬�������� */
	iosm_list_t _listenerList; /* ���������� */
	iosm_list_t _stmList; /* ���Ӷ��� */
	
	resfactory_list_t _responderFactoryList;
	str_int_map_t _connectionIps; /* �ͻ���IP��ַ��(ÿIP��Ӧһ����¼,��������ÿ�ͻ������������ */
	Lock _lock; /* ͬ�����ƶ��� */
	int _tmpFileNameNo; /* ��ʱ�ļ������ */
	char _tmpFileNamePre[5]; /* ��ʱ�ļ�����ǰ׺(����**������**������HTTPServer����ͬһ����ʱĿ¼ʱ��������ͻ��ʹϵͳ�����������) */

	/*
	* ��������
	*/
	std::string _docRoot; /*��Ŀ¼*/
	std::string _tmpRoot; /* ��ʱ�ļ���Ŀ¼ */
	bool _isDirectoryVisible; /*�Ƿ��������Ŀ¼*/
	std::string _dftFileName; /*Ĭ���ļ���*/
	std::string _ip; /*������IP��ַ*/
	u_short _port; /*�����������˿�*/
	size_t _maxConnections; /*���������*/
	size_t _maxConnectionsPerIp; /*ÿ��IP�����������*/
	size_t _maxConnectionSpeed; /*ÿ�����ӵ��ٶ�����,��λ b/s.*/
	unsigned long _sessionTimeout; /*�Ự��ʱ*/
	unsigned long _recvTimeout; /*recv, connect, accept �����ĳ�ʱ*/
	unsigned long _sendTimeout; /*send �����ĳ�ʱ*/
	unsigned long _keepAliveTimeout; /* keep-alive ��ʱ */

	int install(IOAdapter* adp, u_int ev, IOStateMachine* sm, sm_handler_t onStepDone, sm_handler_t onDone, sm_handler_t onAbort);
	int uninstall(IOAdapter* adp);

	/*
	* HTTPServer������Ƶ�״̬��������
	*/
	void handleListener(HTTPListener* listener, IOAdapter* adp, IOStateMachine* sm, stm_result_t* res);
	void handleConnection(HTTPConnection* conn, IOAdapter* adp, IOStateMachine* sm, stm_result_t* res);
	
public:
	HTTPServer();
	~HTTPServer();

	/*
	* IHTTPServer
	*/
	int start(IHTTPConfig *conf);
	int stop();
	void catchRequest(IRequest* req, IResponder** res, IResponderFactory** factory);
	bool mapServerFilePath(const std::string& url, std::string& serverPath);
	std::string tmpFileName();
	inline const std::string& docRoot() { return _docRoot; }
	inline bool isDirectoryVisible() { return _isDirectoryVisible; }
	inline const std::string& defaultFileNames() { return _dftFileName; }
	inline const std::string& ip() { return _ip; }
	inline u_short port() { return _port; }
	inline size_t maxConnectionsPerIp() { return _maxConnectionsPerIp; }
	inline size_t maxConnections() { return _maxConnections; }
	inline size_t maxConnectionSpeed() { return _maxConnectionSpeed; }
	inline unsigned long sessionTimeout() { return _sessionTimeout; }
	inline unsigned long recvTimeout() { return _recvTimeout; }
	inline unsigned long sendTimeout() { return _sendTimeout; }
	inline unsigned long keepAliveTimeout() { return _keepAliveTimeout; }
};

#endif
