/* Copyright (C) 2011 ������
 *
 * ����һ����Դ������,���������ɵ��޸ĺͷ���.
 * ��ֹ������ҵ��;.
 *
 * ��ϵԭ����: querw@sina.com 
*/

#pragma once
#include "fastcgi.h"
#include "ATW.h"
#include "TimerQueue.h"
#include "Pipes/Pipe.h"
#include "IOStateMachine.h"

/*
* ȫ����־
*/
#include "logger.h"
LOGGER_USING(theLogger);

/*
* �������
*/
#define MAX_WINIO 16384 // ϵͳ��������ͬʱʹ�� OSFile() �򿪵��ļ���
#define G_BYTES (1024 * 1024 * 1024) // 1GB
#define M_BYTES (1024 * 1024)		 // 1MB
#define K_BYTES 1024				 // 1KB
#define HOUR (60 * 60)
#define MINUTE 60
#define SECOND 1
#define MAX_DIRECTORY_LIST_SIZE (128 * K_BYTES) // Ŀ¼�б�
#define MAX_SOCKBUFF_SIZE 4096 // ���ջ�����. 
#define FCGI_BUFFER_SIZE 4096 // ����С�� 24, ���� FCGI ����������. һ��Ӧ�ñ�֤��512����.
#define MAX_METHODSIZE 100 // ���ڱ���HTTP�����ַ����ĳ���,���˳��� 200
#define MAX_REQUESTHEADERSIZE (K_BYTES * 4) // �������ͷ��������
#define MAX_RESPONSEHEADERSIZE (K_BYTES * 4) // �������ͷ��������
#define MAX_IP_LENGTH 64 // IP��ַ����󳤶�
#define MIN_SIZE_ONSPEEDLIMITED 512 // �ﵽ�ٶ�����ʱ,���͵���С���ֽ���.
#define MAX_WAITTIME_ONSPEEDLIMITED 2000 // �ﵽ�ٶ�����ʱ,���ȴ����ٺ��뷢��һ����.���ֵ������õù���,�п��ܵ��¿ͻ������������û��Ӧ
#define ETAG_BYTE_SIZE 5 // �����ڴ�����,����ETagʱ��ȡ���ֽ���.
#define FCGI_CONNECT_TIMEO 5000 // ����FCGI�����ܵ��ĳ�ʱʱ��
#define FCGI_MAX_IDLE_SECONDS 5
#define SERVER_SOFTWARE "Q++ HTTP Server/0.20"
#define MAX_POST_DATA (G_BYTES) // ������1G
#define POST_DATA_CACHE_SIZE (8 * K_BYTES) // ����8KB��POST DATA����д���ļ�
#define FCGI_CACHE_SIZE (16 * K_BYTES)
#define FCGI_PIPE_BASENAME "\\\\.\\pipe\\fast_cgi_ques" // FCGI �����ܵ���

/*
* ���������Ͷ���
*/
// HTTP ����
enum HTTP_METHOD
{
	METHOD_UNDEFINE = 0,
	METHOD_GET = 1,
	METHOD_POST,
	METHOD_PUT,
	METHOD_HEAD, // ֻ������Ӧͷ
	METHOD_DELETE, // ɾ��
	METHOD_TRACE,
	METHOD_CONNECT,

	METHOD_UNKNOWN = 100
};

// ��������Ӧ��
#define SC_UNKNOWN 0
#define	SC_OK 200
#define	SC_NOCONTENT 204
#define	SC_PARTIAL 206
#define SC_OBJMOVED 302
#define	SC_BADREQUEST 400
#define	SC_FORBIDDEN 403
#define	SC_NOTFOUND 404
#define	SC_BADMETHOD 405
#define	SC_SERVERERROR 500
#define SC_SERVERBUSY 503

// ����ֵ����(�������Ͷ���)
#define SE_RUNING 1 // ��������
#define SE_STOPPED 2 // �Ѿ�ֹͣ
#define SE_CREATESOCK_FAILED 100 // �׽��ִ���ʧ��
#define SE_CREATETIMER_FAILED 103 // �޷�������ʱ��
#define SE_CREATE_IOCP_FAILED 104
#define SE_INVALID_PARAM 105

#define SE_SUCCESS 0
#define SE_NETWORKFAILD 1  // ��������ģ�鷢������
#define SE_REMOTEFAILD 3// �׽��ֵ�Զ�̶��Ѿ��ر�
#define SE_ACCEPTFAILD 4 // accept ���ص���ʧ��
#define SE_RECVFAILD 5 // recv ����ʧ��
#define SE_SENDFAILD 6 // send ����ʧ��
#define SE_BINDFAILED 7
#define SE_LISTENFAILED 8 
#define SE_UNKNOWN 1000

// �� SE_XXX ������ӳ��Ϊһ�������ַ���
std::string get_error_message(int err);

// HTTP ���Ӷ����˳���
typedef enum HTTP_CLOSE_TYPE
{
	CT_SUCESS = 0,

	CT_CLIENTCLOSED = 10, // �ͻ��˹ر�������
	CT_SENDCOMPLETE, // �������
	CT_SEND_TIMEO,
	CT_RECV_TIMEO,
	CT_SESSION_TIMEO,
	CT_BADREQUEST,
	
	CT_FCGI_SERVERERROR = 20,
	CT_FCGI_CONNECT_FAILED,
	CT_FCGI_SEND_FAILED,
	CT_FCGI_RECV_FAILED,
	CT_FCGI_RECV_TIMEO,
	CT_FCGI_SEND_TIMEO,
	CT_FCGI_ABORT,

	CT_NETWORK_FAILED = 50,
	CT_INTERNAL_ERROR,

	CT_UNKNOWN = 999 // δ֪.	
}HTTP_CONNECTION_EXITCODE;


typedef std::map<std::string, unsigned int> str_int_map_t;
typedef std::vector<std::string> str_vec_t;
typedef void* conn_id_t;
const conn_id_t INVALID_CONNID = NULL;

// �ⲿ������ַ���
extern const char* g_HTTP_Content_NotFound;
extern const char* g_HTTP_Bad_Request;
extern const char* g_HTTP_Bad_Method;
extern const char* g_HTTP_Server_Error;
extern const char* g_HTTP_Forbidden;
extern const char* g_HTTP_Server_Busy;

// ��һ��ʱ���ʽ��Ϊ HTTP Ҫ���ʱ���ʽ(GMT).
std::string format_http_date(__int64* ltime);
std::string to_hex(const unsigned char* pData, int nSize);
std::string decode_url(const std::string& inputStr);
bool map_method(HTTP_METHOD md, char *str);
int http_header_end(const char *data, size_t len);
std::string get_field(const char* buf, const char* key);
void get_file_ext(const std::string &fileName, std::string &ext);
bool match_file_ext(const std::string &ext, const std::string &extList);
std::string get_last_error(DWORD errCode = 0);
size_t split_strings(const std::string &str, str_vec_t &vec, const std::string &sp);
bool get_ip_address(std::string& str);
std::string format_time(size_t ms);
std::string format_size(__int64 bytes);
std::string format_speed(__int64 bytes, unsigned int timeUsed);


// Fast CGI ����������
typedef struct FCGI_SERVER_CONTEXT
{
	char name[MAX_PATH + 1];
	bool status;
	char path[MAX_PATH + 1]; // ip��ַ(Զ��ģʽ)����������(����ģʽ)
	u_short port; // �˿�. 0��ʾ�Ǳ���ģʽ
	char exts[MAX_PATH + 1]; // �ļ���չ��,���ŷָ�
	size_t maxConnections; // ���������
	size_t maxWaitListSize; // �ȴ����д�С
	int cacheAll;	// �Ƿ񻺴�����
}fcgi_server_ctx_t;

/*
* HTTP ���ýӿ�
*/
class IHTTPConfig
{
public:
	IHTTPConfig() {};
	virtual ~IHTTPConfig() {};

	virtual std::string docRoot() = 0;
	virtual std::string tmpRoot() = 0;
	virtual std::string defaultFileNames() = 0;
	virtual std::string ip() = 0;
	virtual u_short port() = 0;
	virtual bool dirVisible() = 0;
	virtual size_t maxConnections() = 0;
	virtual size_t maxConnectionsPerIp() = 0;
	virtual size_t maxConnectionSpeed() = 0;
	virtual size_t sessionTimeout() = 0;
	virtual size_t recvTimeout() = 0;
	virtual size_t sendTimeout() = 0;
	virtual size_t keepAliveTimeout() = 0;
	virtual bool getFirstFcgiServer(fcgi_server_ctx_t *serverInf) = 0;
	virtual bool getNextFcgiServer(fcgi_server_ctx_t *serverInf) = 0;
};

class IRequest : public IOStateMachine
{
public:
	virtual ~IRequest(){};

	virtual bool isValid() = 0;
	virtual HTTP_METHOD method() = 0;
	virtual std::string uri(bool decode) = 0;
	virtual std::string field(const char* key) = 0;
	virtual bool keepAlive() = 0;
	virtual bool range(__int64 &from, __int64 &to) = 0;
	virtual size_t contentLength() = 0;
	virtual void statistics(__int64* bytesRecved, __int64* bytesSent) = 0;

	virtual std::string getHeader() = 0;
	virtual IPipe* getPostData() = 0;

	virtual void nextRequest() = 0;
};

class IResponder : public IOStateMachine
{
public:
	virtual ~IResponder() {};

	/* ������Ӧͷ */
	virtual IRequest* getRequest() = 0;
	virtual std::string getHeader() = 0;
	virtual int getServerCode() = 0;
	virtual void statistics(__int64* bytesRecved, __int64* bytesSent) = 0;
};

class IResponderFactory
{
public:
	IResponderFactory(){};
	virtual ~IResponderFactory(){};

	virtual IResponder* catchRequest(IRequest* req) = 0;
	virtual void releaseResponder(IResponder* res) = 0;
};

/*
* HTTP Server �ĳ���ӿ�
*/
class IHTTPServer
{
public:
	virtual ~IHTTPServer() {};
	virtual int start(IHTTPConfig *conf) = 0;
	virtual int stop() = 0;

	/*
	* ��׽����
	*/
	virtual void catchRequest(IRequest* req, IResponder** res, IResponderFactory** factory) = 0;

	/*
	* ��ȡSERVER��Ϣ
	*/
	virtual bool mapServerFilePath(const std::string& url, std::string& serverPath) = 0;
	virtual std::string tmpFileName() = 0;
	virtual const std::string& docRoot() = 0;
	virtual bool isDirectoryVisible() = 0;
	virtual const std::string& defaultFileNames() = 0;
	virtual const std::string& ip() = 0;
	virtual u_short port() = 0;
	virtual size_t maxConnectionsPerIp() = 0;
	virtual size_t maxConnections() = 0;
	virtual size_t maxConnectionSpeed() = 0;
	virtual unsigned long sessionTimeout() = 0;
	virtual unsigned long recvTimeout() = 0;
	virtual unsigned long sendTimeout() = 0;
	virtual unsigned long keepAliveTimeout() = 0;
};

/* �ⲿʹ�õĹ������� */
extern IHTTPConfig* create_http_config_xml(const char* fileName);
extern void destroy_http_config_xml(IHTTPConfig* conf);
extern IHTTPServer* create_http_server();
extern void destroy_http_server(IHTTPServer* svr);