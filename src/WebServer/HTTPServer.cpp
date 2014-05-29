/* Copyright (C) 2011 ������
 *
 * ����һ����Դ������,���������ɵ��޸ĺͷ���.
 * ��ֹ������ҵ��;.
 *
 * ��ϵԭ����: querw@sina.com 
*/

#include "stdafx.h"
#include <assert.h>
#include <io.h>
#include <algorithm>
#include "HttpServer.h"
#include "HTTPRequest.h"
#include "HTTPResponder.h"
#include "FCGIResponder.h"
#include "HTTPResponderFactory.h"
#include "HTTPConnection.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
HTTPServer::HTTPServer()
{
}

HTTPServer::~HTTPServer()
{
}

bool HTTPServer::mapServerFilePath(const std::string& orgUrl, std::string& serverPath)
{
	// ��url���� utf-8 ����
	std::string deUrl = decode_url(orgUrl);

	// ��ø�Ŀ¼
	serverPath = _docRoot;
	if(serverPath.back() == '\\') serverPath.erase(--serverPath.end());

	// �� URL �е�·������(�������ֺ���)�ϲ��������·��
	std::string::size_type pos = orgUrl.find('?');
	if( pos != std::string::npos )
	{
		serverPath += orgUrl.substr(0, pos);
	}
	else
	{
		serverPath += deUrl;
	}
	
	// URL����б���滻Ϊ��б��.
	for(std::string::iterator iter = serverPath.begin(); iter != serverPath.end(); ++iter)
	{
		if( *iter == '/' ) *iter = '\\'; 
	}

	// �����Ŀ¼�����Ҳ��������Ŀ¼,�������Ĭ���ļ���
	if(serverPath.back() == '\\' && !isDirectoryVisible())
	{
		// ��ֹ���Ŀ¼,�ȳ��Դ�Ĭ���ļ�
		bool hasDftFile = false;
		str_vec_t dftFileNames;
		split_strings(defaultFileNames(), dftFileNames, ",");
		for(str_vec_t::iterator iter = dftFileNames.begin(); iter != dftFileNames.end(); ++iter)
		{
			std::string dftFilePath(serverPath);
			dftFilePath += *iter;
			if(OSFile::exist(dftFilePath.c_str()))
			{
				serverPath += *iter;
				hasDftFile = true;
				break;
			}
		}

		return hasDftFile;
	}
	else
	{
		return true;
	}
}

std::string HTTPServer::tmpFileName()
{
	char fileName[MAX_PATH + 1] = {0};
	if( 0 == GetTempFileNameA(_tmpRoot.c_str(), _tmpFileNamePre, 0, fileName))
	{
		assert(0);

		/* �޷���ȡ��ʱ�ļ���,���������һ�� */
		int no = 0;
		_lock.lock();
		no = ++_tmpFileNameNo;
		_lock.unlock();

		std::stringstream fmt;

		if(_tmpRoot.back() == '\\')
		{
			fmt << _tmpRoot << no << ".tmp";
		}
		else
		{
			fmt << _tmpRoot << '\\' << no << ".tmp";
		}

		return fmt.str();
	}
	else
	{
		return fileName;
	}
}

void HTTPServer::catchRequest(IRequest* request, IResponder** responder, IResponderFactory** factory)
{
	// ����һ�� IResponder,��������adp�ϰ�װ.
	if(request->isValid())
	{
		// ������Ч����,ʹ��Ĭ�ϵ�HTTPResponder����һ�� 400 ��Ӧ.
		*factory = _responderFactoryList.back();
		*responder = (*factory)->catchRequest(request);
	}
	else
	{
		*responder = NULL;
		*factory = NULL;
		for(auto itr = _responderFactoryList.begin(); itr != _responderFactoryList.end(); ++itr)
		{
			IResponderFactory *fty = *itr;
			IResponder* res = fty->catchRequest(request);
			if(res)
			{
				*factory = fty;
				*responder = res;
				break;
			}
		}
	}
}

int HTTPServer::start(IHTTPConfig *conf)
{
	int ret = SE_SUCCESS;

	do
	{
		/*
		* �������ò���
		*/
		_docRoot = conf->docRoot(); /*��Ŀ¼*/
		_tmpRoot = conf->tmpRoot();
		_isDirectoryVisible = conf->dirVisible(); /*�Ƿ��������Ŀ¼*/
		_dftFileName = conf->defaultFileNames(); /*Ĭ���ļ���*/
		_ip = conf->ip(); /*������IP��ַ*/
		_port = conf->port(); /*�����������˿�*/
		_maxConnections = conf->maxConnections(); /*���������*/
		_maxConnectionsPerIp = conf->maxConnectionsPerIp(); /*ÿ��IP�����������*/
		_maxConnectionSpeed = conf->maxConnectionSpeed(); /*ÿ�����ӵ��ٶ�����,��λ b/s.*/

		_sessionTimeout = conf->sessionTimeout(); /*�Ự��ʱ*/
		_recvTimeout = conf->recvTimeout(); /*recv, connect, accept �����ĳ�ʱ*/
		_sendTimeout = conf->sendTimeout(); /*send �����ĳ�ʱ*/
		_keepAliveTimeout = conf->keepAliveTimeout();

		/*
		* ����IO������
		*/
		_scheduler = create_scheduler();
		if(NULL == _scheduler)
		{
			assert(0);
			ret = SE_NETWORKFAILD;
			LOGGER_CERROR(theLogger, _T("�޷���ʼ��������.\r\n"));
			break;
		}

		/*
		* ��ʼ�� Responder Factory List
		*/
		IResponderFactory *dftFactory = new DefaultResponderFactory(this);
		_responderFactoryList.push_back(dftFactory);

		/*
		* ������������
		*/
		IOAdapter* listenAdp = IO_create_adapter();
		if(IO_ERROR_SUCESS != listenAdp->bind(ip().c_str(), port()))
		{
			IO_destroy_adapter(listenAdp);
			LOGGER_CERROR(theLogger, _T("�޷��󶨶˿�:%d.\r\n"), port());
			ret = SE_BINDFAILED;
			break;
		}
		else if(IO_ERROR_SUCESS != listenAdp->listen())
		{
			IO_destroy_adapter(listenAdp);
			LOGGER_CERROR(theLogger, _T("�޷������˿�[%d].\r\n"), port());
			ret = SE_LISTENFAILED;
			break;
		}

	
		/*
		* ��װ��һ��״̬��(������)��ʼ�ṩHTTP����
		*/
		HTTPListener* listener = new HTTPListener();
		install(
			listenAdp,
			IO_EVENT_EPOLLIN,
			listener,
			std::bind(&HTTPServer::handleListener, this, listener, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3),
			std::bind(&HTTPServer::handleListener, this, listener, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3),
			std::bind(&HTTPServer::handleListener, this, listener, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
			);
		_listenerList.push_back(std::make_pair(listenAdp, listener));

		/*
		* ��������ʼ����
		*/
		if(0 !=_scheduler->start(GetSystemProcessorNumber() * 2))
		{
			assert(0);
			ret = SE_NETWORKFAILD;
			LOGGER_CERROR(theLogger, _T("�������޷�����.\r\n"));
			break;
		}

		/*
		* �ɹ��ĳ���
		*/
		std::string ipAddress = conf->ip();
		if(ipAddress == "")
		{
			get_ip_address(ipAddress);
		}
		LOGGER_CINFO(theLogger, _T("Q++ HTTP Server ��������,��Ŀ¼[%s],��ַ[%s:%d],���������[%d].\r\n"), 
			AtoT(docRoot()).c_str(), AtoT(ipAddress).c_str(), port(), maxConnections());

		return SE_SUCCESS;
	}while(0);

	/*
	* ʧ�ܵĳ���
	*/
	stop();
	LOGGER_CWARNING(theLogger, _T("Q++ HTTP Server ����ʧ��.\r\n"));
	return ret;
}

int HTTPServer::stop()
{
	if(_scheduler)
	{
		// ֹͣ������
		_scheduler->stop();

		// ɾ�����е���������
		for(auto itr = _listenerList.begin(); itr != _listenerList.end(); ++itr)
		{
			uninstall(itr->first);
		}
		_listenerList.clear();

		// ɾ������״̬��
		if(_stmList.size() > 0)
		{
			LOGGER_CWARNING(theLogger, _T("�˳�ʱ��[%d]δж�ص�״̬��(����).\r\n"), _stmList.size());
		}
		for(auto itr = _stmList.begin(); itr != _stmList.end(); ++itr)
		{
#ifdef DEBUG
			LOGGER_DEBUG(theLogger, _T("ǿ��ж��״̬��") << AtoT(typeid(*(itr->second)).name()) << _T("\r\n")); 
#endif
			// ж��״̬��
			_scheduler->uninstall(itr->first);

			// �ر� IOAdapter 
			IO_destroy_adapter(itr->first);

			// ɾ��״̬��
			delete itr->second;
		}
		_stmList.clear();

		// ɾ�� Responder Factory
		for(auto itr = _responderFactoryList.begin(); itr != _responderFactoryList.end(); ++itr)
		{
			delete *itr;
		}
		_responderFactoryList.clear();

		destroy_scheduler(_scheduler);
		_scheduler = NULL;

		LOGGER_CINFO(theLogger, _T("Q++ HTTP Server ֹͣ.\r\n"));
	}
	return SE_SUCCESS;
}

class stmRemoveFunctor
{
private:
	IOAdapter* _adp;
public:
	stmRemoveFunctor(IOAdapter* adp) : _adp(adp) {};
	bool operator () (const std::pair<IOAdapter*, IOStateMachine*>& val)
	{
		return val.first == _adp;
	}
};

int HTTPServer::install(IOAdapter* adp, u_int ev, IOStateMachine* stm, sm_handler_t onStepDone, sm_handler_t onDone, sm_handler_t onAbort)
{
	_lock.lock();
	_stmList.push_back(std::make_pair(adp, stm));
	_lock.unlock();

	_scheduler->install(adp, ev, stm, onStepDone, onDone, onAbort);

	return 0;
}

int HTTPServer::uninstall(IOAdapter* adp)
{
	IOStateMachine* stm = NULL;
	_scheduler->uninstall(adp);

	_lock.lock();
	iosm_list_t::iterator itr = std::find_if(_stmList.begin(), _stmList.end(), stmRemoveFunctor(adp));
	if(itr == _stmList.end())
	{
		assert(0);
	}
	else
	{
		stm = itr->second;
		_stmList.erase(itr);
	}
	_lock.unlock();

	if(stm)
	{
		delete stm;
		IO_destroy_adapter(adp);
	}
	return 0;
}

/*
* ����һ��������
*/
void HTTPServer::handleListener(HTTPListener* listener, IOAdapter* adp, IOStateMachine* sm, stm_result_t* res)
{
	if(res->rc == STM_ABORT)
	{
		assert(0);
		LOGGER_ERROR(theLogger, _T("�����׽��ֳ���.\r\n"));
	}
	else
	{
		IOAdapter* newAdp = listener->getAdapter();
		HTTPConnection *conn = new HTTPConnection(this, newAdp);
		LOGGER_INFO(theLogger, _T("[") << AtoT(conn->getRemoteIPAddr()) << _T(":") << conn->getRemotePort() << _T("] - �����ӱ�����.") << _T("\r\n"));

		// ��װһ�� CONNECTION ״̬������HTTP����
		// ��ʼ�¼�����Ϊ IO_EVENT_EPOLLOUT ������ IO_EVENT_EPOLLIN ����Ϊ��Ҫ connection ��������һ��. �����ӵ�IOAdapter���ǿ�д,�������� IO_EVENT_EPOLLOUT �����õ��������ϵ��� conn ����һ��.
		install(newAdp, IO_EVENT_EPOLLOUT, conn,
			std::bind(&HTTPServer::handleConnection, this, conn, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3),
			std::bind(&HTTPServer::handleConnection, this, conn, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3),
			std::bind(&HTTPServer::handleConnection, this, conn, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	}
}

void HTTPServer::handleConnection(HTTPConnection* conn, IOAdapter* adp, IOStateMachine* sm, stm_result_t* res)
{
	if(STM_STEPDONE == res->rc)
	{
		request_statis_t requestInfo;
		conn->queryLastRequest(&requestInfo);
		if(res->st == 1)
		{
			// ����ʼ
			char method[100];
			map_method(requestInfo.method, method);
			LOGGER_INFO(theLogger, _T("[") << AtoT(conn->getRemoteIPAddr()) << _T(":") << conn->getRemotePort() << _T("] - [") << method << _T("][") \
				<< AtoT(requestInfo.uri) << _T("]...") << _T("\r\n"));
		}
		else
		{
			// ���������
			LOGGER_INFO(theLogger, _T("[") << AtoT(conn->getRemoteIPAddr()) << _T(":") << conn->getRemotePort() << _T("] - [") << AtoT(requestInfo.uri) << _T("]")\
				<< _T(" -> [") << requestInfo.sc << _T("],����[") << AtoT(format_size(requestInfo.bytesRecved)) << _T("],����[") << AtoT(format_size(requestInfo.bytesSent)) << _T("]")\
				<< _T(",��ʱ[") << AtoT(format_time(requestInfo.usedTime)) << _T("],ƽ������[") << AtoT(format_speed(requestInfo.bytesSent, requestInfo.usedTime)) << _T("].")\
				<< _T("\r\n"));

			// setprecision(3) <iomanip>
		}
	}
	else if(STM_DONE == res->rc)
	{
		// ���ӹر�
		connection_statis_t connInfo;
		conn->query(&connInfo);

		LOGGER_INFO(theLogger, _T("[") << AtoT(conn->getRemoteIPAddr()) << _T(":") << conn->getRemotePort() << _T("] - ���ӹر�,�ܼ�:")\
			<< _T("��������[") << connInfo.requestCount << _T("]")\
			<< _T(",����[") << AtoT(format_size(connInfo.bytesRecved)) << _T("],����[") << AtoT(format_size(connInfo.bytesSent)) << _T("]")\
			<< _T(",��ʱ[") << AtoT(format_time(connInfo.usedTime)) << _T("],ƽ������[") << AtoT(format_speed(connInfo.bytesSent, connInfo.usedTime)) << _T("].")\
			<< _T("\r\n"));

		uninstall(adp);
	}
	else if(STM_ABORT == res->rc)
	{
		/*
		* HTTP���������ڴ���HTTP����,ֻ�����������Żᵼ��HTTP���ӳ���,�����ڳ���������(STM_ABORT)����Ҫͬʱ��ʾһ��������ֹ����Ϣ��һ��������ֹ����Ϣ
		*/

		// get_error_message ӳ�� SE_XXX �Ĵ����ַ���
		// ..
		// ..

		// ��������ֹ
		request_statis_t requestInfo;
		conn->queryLastRequest(&requestInfo);
		LOGGER_INFO(theLogger, _T("[") << AtoT(conn->getRemoteIPAddr()) << _T(":") << conn->getRemotePort() << _T("] - ������ֹ:[") << AtoT(requestInfo.uri) << _T("]")\
			<< _T(" -> [") << requestInfo.sc << _T("],����[") << AtoT(format_size(requestInfo.bytesRecved)) << _T("],����[") << AtoT(format_size(requestInfo.bytesSent)) << _T("]")\
			<< _T(",��ʱ[") << AtoT(format_time(requestInfo.usedTime)) << _T("],ƽ������[") << AtoT(format_speed(requestInfo.bytesSent, requestInfo.usedTime)) << _T("].")\
			<< _T("\r\n"));

		// ������ֹ
		connection_statis_t connInfo;
		conn->query(&connInfo);

		LOGGER_INFO(theLogger, _T("[") << AtoT(conn->getRemoteIPAddr()) << _T(":") << conn->getRemotePort() << _T("] - ������ֹ,�ܼ�:")\
			<< _T("����[") << connInfo.requestCount << _T("]")\
			<< _T(",����[") << AtoT(format_size(connInfo.bytesRecved)) << _T("],����[") << AtoT(format_size(connInfo.bytesSent)) << _T("]")\
			<< _T(",��ʱ[") << AtoT(format_time(connInfo.usedTime)) << _T("].")\
			<< _T("\r\n"));

		uninstall(adp);
	}
	else
	{
		assert(0);
	}
}