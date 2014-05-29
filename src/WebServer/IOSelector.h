#pragma once

/* Copyright (C) 2014 ������
 *
 * ����һ����Դ������,���������ɵ��޸ĺͷ���.
 * ��ֹ������ҵ��;.
 *
 * ��ϵԭ����: querw@sina.com 
*/

/*
 * V0.3
 *
 * ����ģ������Ŀ����ʵ��һ�����һ�µĽӿڷ�װ����ƽ̨�Ƚ������IO����ģʽ, Windows IOCP �� Linux EPOLL,������Ϊ������Unix epoll �� ET ģʽ.
 * Windows IOCPʵ�ּ򵥵�˵����Ϊÿ���׽��ֵ���������ջ���ͷ��ͻ���,�������ύ���ݺ�,��̨(IOCPSelector)���߳�ˢ��2��������,��������ʱ�����ɶ�/
 * ��д�¼�.������ͨ�� IOSelector ��epoll�����õ���Ծ(���¼�������)�� IOAdapter.
 * ��ҪĿ���Ǳ���ص����������,���˲�ϲ���ص�����,���뱻�����ио��ϲ�������.
 *
 * IOAdapter ��װ���׽��ֵ�ȫ��ϸ��
 * IOSelector��װ������ģ�͵�ϸ��.
 *
 * ����˵��:
 * 1. ͨ��accept�õ���IOAdapter��ʼ���ڿɶ���д��״̬.
 * 2. ���� recv ���� IO_ERROR_SUCESS ��ʾ�����Ѿ������Ƶ�ָ����������,������len����ָ��; ����IO_ERROR_RECVPENDING��ʾIO�������ڽ���; ����
 *    IO_ERROR_RECVFAILD ��ʾ�׽����쳣.
 * 3. ����send��recv���������.
 *
 * ��������: 
 * 1. һ��IOAdapter ������������һ�� RECV IO �� һ�� SEND IO ͬʱ���л���ֻ����һ�� IO(�ɺ궨�� DOUBLEIO ����). ������Ӧ��ȷ����һ��(������
 *    �߼�Ӧ��ȷ��ֻ�����յ��ɶ�/��д֪ͨ��,����recv/send����ֱ������IOPENDINGΪֹ).
 * 2. IOAdapter �� IOSelector����,����Ӧ����ɾ�����е� IOAdapter ��ɾ��IOSelector.
 * 
 * by Q++ Studio 2014.3.25

 * IOCPSelector �� IOCPAdapter ��һ���,�߼��ϲ�������ɢ����Ϲ�ϵ���ǽ����,ֻ���ṩ��һ��ͳһ�ӿڵ���������,�Է���ʹ��.
 * �� ������ recv ��һ��㲻һ���ĵط�: recv ����0ʱ,����getLastError ���� IO_ERROR_SUCESS ��ʾû�д�����,ֻ�ǻ�������û��������,�����ṩһ�������� EAGAIN ���� WSA_IO_PENDING֮��Ĵ�����.
 * 
*/

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
* ������IPipe�ӿ���֧��Q++ HTTP Server V0.3 �ܵ�ϵͳ
* ʵ��IPipe�ӿڿ���ֱ�ӷ���IOAdapter�ڲ����׽��ֻ������Խ�ʡ�ڴ�
* �粻��Ҫ������ܰ� #define DERIVE_FROM_PIPE ɾ������
*/
#define DERIVE_FROM_PIPE
#ifdef DERIVE_FROM_PIPE
#include "Pipes/Pipe.h"
#endif

/*
* ���ߺ꺯��,����֪��
*/
#define TEST_BIT(val, bit) (val & bit)
#define SET_BIT(val, bit) (val |= bit)
#define UNSET_BIT(val, bit) (val &= ~bit)

/*
* Last Error Type define
* Windowsϵͳ���� WSAGetLastError �ĸ���(���ֱ��ش��󱻺ϲ�). Linuxϵͳ���Զ���.
*/
#define IO_ERROR_SUCESS 0
#define IO_ERROR_BINDFAILED 1			// ��ʧ��,�˿ڱ�ռ��.
#define IO_ERROR_LISTENFAILED 2			// ����ʧ��,�Ѿ����ӻ����������ش���.
#define IO_ERROR_ACCEPTFAILED 3			// ����ʧ��,�޷����AcceptExָ������������ش���.
#define IO_ERROR_RECVFAILED 4			// ����ʧ��,���ش���.
#define IO_ERROR_SENDFAILED 5			// ����ʧ��,���ش���.
#define IO_ERROR_CONNECTFAILED 6		// ����ʧ��,���ش���.
#define IO_ERROR_BUSY 10				// �������ڽ�����.
#define IO_ERROR_HUP 11					// �����ѶϿ�.
#define IO_ERROR_SHUTDOWN 30			// ������SHUTDOWN���.
#define IO_ERROR_UNDEFINED 100

/*
* �׽��ַ�װ�ӿ�
*/
class IOAdapter : public IUserDataContainer

#ifdef DERIVE_FROM_PIPE
	, public IPipe
#endif

{
protected:
	IOAdapter(){};
	virtual ~IOAdapter(){};

public:
	virtual int setopt(int optname, const char* optval, int optlen) = 0;
	virtual int getopt(int optname, char* optval, int* optlen) = 0;
	virtual int getsockname(char *ipAddr, u_short &port) = 0;
	virtual int getpeername(char* ipAddr, u_short &port) = 0;
	virtual int getLastError() = 0;
	virtual int query(__int64* bytesReceived, __int64* bytesSent) = 0;	
	virtual int shutdown(int how) = 0;

	virtual int bind(const char* ipAddr, u_short port) = 0;
	virtual int listen(int backlog = SOMAXCONN) = 0;	
	virtual IOAdapter* accept() = 0;
	virtual int connect(const char* ipAddr, u_short port) = 0;
	virtual int recv(void *buf, size_t len) = 0;
	virtual int send(const void *buf, size_t len) = 0;
};


/*
* ctl ������OPP�����EVENT����
*/
#define IO_CTL_ADD 0
#define IO_CTL_MOD 1
#define IO_CTL_DEL 2

#define IO_EVENT_NONE 0
#define IO_EVENT_EPOLLIN 0x01 /* recv or accept available */
#define IO_EVENT_EPOLLOUT 0x02 /* send available or connect done */
#define IO_EVENT_EPOLLERR 0x04 /* error occur */
#define IO_EVENT_EPOLLHUP 0x08 /* auto set */
#define IO_EVENT_EPOLLET 0x10
#define IO_EVENT_EPOLLONESHOT 0x20
#define IO_EVENT_AUTODELETE 0x80 /* �ڲ�ʹ��*/

/*
* wait() ����ֵ
*/
#define IO_WAIT_SUCESS 0 /* ȡ����һ����ԾIOAdpater */
#define IO_WAIT_TIMEOUT 1 /* ��ʱ */
#define IO_WAIT_ERROR 2 /* ���� */
#define IO_WAIT_EXIT 3 /* �˳� */

/*
* ѡ������װ�ӿ�
*/
class IOSelector
{
protected:
	IOSelector(){};
	virtual ~IOSelector(){};

public:
	virtual int ctl(IOAdapter* adp, int op, unsigned int ev) = 0;
	virtual int wait(IOAdapter** adp, unsigned int* ev, unsigned int timeo = INFINITE) = 0;
	virtual int wakeup(size_t counts) = 0;
};

/*
* ֻ����ͨ�������ഴ�� selector ʵ��
*/
extern IOSelector* IO_create_selector();
extern int IO_destroy_slector(IOSelector* s);
extern IOAdapter* IO_create_adapter();
extern IOAdapter* IO_create_adapter(const char* filename);
extern int IO_destroy_adapter(IOAdapter* adp);
extern bool IO_formate_error_message(int errorCode, std::string* msg);