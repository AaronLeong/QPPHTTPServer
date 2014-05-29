#include "stdafx.h"
#include "IOCP.h"

/************************************************************************/
/*  IOCPָ���ʵ��                                                      */
/************************************************************************/

static bool is_socket_listening(SOCKET s)
{
	BOOL listening = FALSE;
	int len = sizeof(BOOL);
	getsockopt(s, SOL_SOCKET, SO_ACCEPTCONN, (char*)&listening, &len);

	return listening == TRUE;
}

static bool is_socket_connected(SOCKET s)
{
	sockaddr addr;
	int len = sizeof(sockaddr);
	return 0 == getpeername(s, &addr, &len);
}

static int get_socket_type(SOCKET s)
{
	int sockType = SOCK_STREAM;
	int len = sizeof(sockType);
	if(SOCKET_ERROR == getsockopt(s, SOL_SOCKET, SO_TYPE, (char*)&sockType, &len))
	{
		if(WSAENOTSOCK == WSAGetLastError())
		{
			sockType = SOCK_FILE;
		}
		else
		{
			assert(0);
		}
	}
	return sockType;
}


/*
* IOCPSelector �� IOCPAdapter ���� IOCPContext, IOCPContext �� IOCPSelector ά��, ctl ADDʱ����,������һ�� IOCPAdapter.
* IOCPAdapter ���ر�ʱ�ջ�. IOCPAdapter ֻ��һ�������ӿ�.
*/
IOCPContext::IOCPContext(SOCKET s, int sockType, size_t recvBufLen, size_t sendBufLen)
{
	_sock = s;
	_sockType = sockType;
	if(SOCK_FILE != _sockType)
	{
		_listening = is_socket_listening(s);
		_connected = is_socket_connected(s);
	}
	else
	{
		_listening = false;
		_connected = true;
	}

	_lock = NULL;
	_mode = IO_MODE_LT;
	_lastErrorCode = IO_ERROR_SUCESS;
	
	_newSock = INVALID_SOCKET;
	_acceptBuf = NULL;
	_lpfnAcceptEx = NULL;
	_shutdownFlag = -1;

	memset(&_sendOlp, 0, sizeof(iocp_overlapped_t));
	memset(&_recvOlp, 0, sizeof(iocp_overlapped_t));
	_recvOlp.len = recvBufLen;
	_sendOlp.len = sendBufLen;

	adapter = NULL;
	isInQueue = false;
	epollEvent = IO_EVENT_NONE;
	curEvent = IO_EVENT_NONE;
	isAutoDelete = false;
}

IOCPContext::~IOCPContext()
{
	if(_newSock != INVALID_SOCKET) closesocket(_newSock);
	if(_acceptBuf) delete []_acceptBuf;
	if(_recvOlp.buf) delete []_recvOlp.buf;
	if(_sendOlp.buf) delete []_sendOlp.buf;
}

Lock* IOCPContext::setLock(Lock* l)
{
	Lock* oldLock = _lock;
	_lock = l;
	return oldLock;
}

void IOCPContext::lock()
{
	if(_lock) _lock->lock();
}

void IOCPContext::unlock()
{
	if(_lock) _lock->unlock();
}

bool IOCPContext::busy()
{
	return _recvOlp.oppType != IO_OPP_NONE || _sendOlp.oppType != IO_OPP_NONE;
}

int IOCPContext::setMode(int m)
{
	int oldm = _mode;
	_mode = m;
	return oldm;
}

int IOCPContext::setLastError(int err)
{
	/* 
	* ֻ�з�������ż�¼,�� WSAGetLastError ������һ�� 
	* The WSAGetLastError function returns the error status for the last Windows Sockets operation that failed.
	*/
	int olderr = _lastErrorCode;
	if(err != IO_ERROR_SUCESS)
	{
		_lastErrorCode = err;
	}
	return olderr;
}

int IOCPContext::getLastError()
{
	return _lastErrorCode;
}

bool IOCPContext::ready(int t)
{
	if(IO_READYTYPE_EPOLLIN == t)
	{
		if(_listening)
		{
			// �����׽���:�Ѿ���һ����Ч��sock acceptIO���
			return _recvOlp.oppType == IO_OPP_NONE && _newSock != INVALID_SOCKET;
		}
		else
		{
			// ���ջ�������δ������->��Ϊ���ھ���״̬. [start ->(�Ѷ�) -> upos -> (δ��) -> ipos (���л�IOCP������) -> end]
			return _recvOlp.upos < _recvOlp.ipos;
		}
	}
	else
	{
		if(_listening)
		{
			// �����׽��ֲ���д
			return false;
		}
		else
		{
			// �Ѿ����Ӳ��ҷ��ͻ����пռ�->��Ϊ���ھ���״̬ [start -> (���ڷ���) -> ipos -> (��д��δ����) -> upos -> (����) -> end]
			return _connected && _sendOlp.upos < _sendOlp.len;
		}
	}
}

u_int IOCPContext::updateEvent(u_int ev)
{
	u_int curEv = IO_EVENT_NONE;
	
	/*
	* ������Ҫ���� ��һ��IOCP ����.
	*/
	if(ev != IO_EVENT_NONE)
	{
		// ���ù���ģʽ
		setMode(TEST_BIT(ev, IO_EVENT_EPOLLET) ? IO_MODE_ET : IO_MODE_LT);
		
		// ������ EPOLLIN, EPOLLOUT ����Ҫ�ֶ���ʼ,���ö�Ӧ�ĺ�����Ȼ�ᷢ��IOCP����.
		if(getLastError() == IO_ERROR_SUCESS)
		{
			if(TEST_BIT(ev, IO_EVENT_EPOLLIN))
			{
				if(_listening)
				{
					setLastError(postAccept());
				}
				else
				{
					setLastError(postRecv());
				}
			}
		}
	}

	/*
	* ���ݵ�ǰ״̬�����Ҫ�������¼�
	*/
	// ֻҪ��δ������/���������׽��־�������Ϊ"�ɶ�"�����׽����Ƿ����.
	if(ready(IO_READYTYPE_EPOLLIN))
	{
		SET_BIT(curEv, IO_EVENT_EPOLLIN);
	}

	// ֻ���׽���״̬������"��д"
	int err = getLastError();
	if(IO_ERROR_SUCESS == err)
	{
		if(ready(IO_READYTYPE_EPOLLOUT))
		{
			SET_BIT(curEv, IO_EVENT_EPOLLOUT);
		}
	}
	else if(IO_ERROR_HUP == err)
	{
		SET_BIT(curEv, IO_EVENT_EPOLLHUP);
	}
	else
	{
		SET_BIT(curEv, IO_EVENT_EPOLLERR);
	}
	return curEv;
}

u_int IOCPContext::onIoCompleted(bool oppResult, IOCPOVERLAPPED* olp, size_t bytesTransfered)
{
	u_int ev = IO_EVENT_NONE;

	/*
	* �����Ѿ���ɵĲ���:ͳ���ֽ����ȵ�.
	*/
	if(oppResult)
	{
		olp->transfered += bytesTransfered;
	}

	/* ��ձ�� */
	int oppType = olp->oppType;
	olp->oppType = IO_OPP_NONE;

	/*
	*  ���ദ���Ѿ���ɵĲ������
	*/
	if(_sock == INVALID_SOCKET)
	{
		ev = IO_EVENT_AUTODELETE;
	}
	else
	{
		switch(oppType)
		{
		case IO_OPP_ACCEPT:
			{
				if(oppResult)
				{
					/* �����׽��ֵ�ַ */
					if( 0 != setsockopt( _newSock, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, (char *)&_sock, sizeof(_sock)) )
					{
						assert(0);
					}
					/* �����ɶ��¼�(����IOSelector�Ŀɶ�����) */
					ev = IO_EVENT_EPOLLIN;
				}
				else
				{
					/*
					* WSAECONNRESET: Զ�����Ӻ��ֱ��ر�.
					*/
					closesocket(_newSock);
					_newSock = INVALID_SOCKET;
					ev = IO_EVENT_EPOLLHUP;
				}
			}
			break;
		case IO_OPP_CONNECT:
			{
				/* ���ӳɹ��򴥷���д�¼�,���򴥷������¼�(������Ӧ����ʶ�����ʱ�ĳ����¼�ָ����ʧ��) */
				if(oppResult)
				{
					ev = IO_EVENT_EPOLLOUT;
					_connected = true;
				}
				else
				{
					ev = IO_EVENT_EPOLLHUP;
				}
			}
			break;
		case IO_OPP_RECV:
			{
				if(oppResult && bytesTransfered > 0)
				{
					_recvOlp.ipos += bytesTransfered;
					if(_mode == IO_MODE_ET)
					{
						if(_recvOlp.et)
						{
							ev = IO_EVENT_EPOLLIN;
							_recvOlp.et = false;
						}
						else
						{
							ev = IO_EVENT_NONE;
						}
					}
					else
					{
						ev = IO_EVENT_EPOLLIN;
					}

					/* ������������,�������ʧ��(һ�����ش���)��ͬʱ����ERR�¼� */
					int drr = postRecv();
					if(drr != IO_ERROR_SUCESS)
					{
						setLastError(drr);
						SET_BIT(ev, IO_EVENT_EPOLLERR);
					}
				}
				else
				{
					/* ����error�¼� */
					ev = IO_EVENT_EPOLLHUP;
					setLastError(IO_ERROR_HUP);
				}
			}
			break;
		case IO_OPP_SEND:
			{
				if(oppResult && bytesTransfered > 0)
				{
					_sendOlp.ipos = bytesTransfered;

					if(_mode == IO_MODE_ET)
					{
						if(_sendOlp.et)
						{
							ev = IO_EVENT_EPOLLOUT;
							_sendOlp.et = false;
						}
						else
						{
							ev = IO_EVENT_NONE;
						}
					}
					else
					{
						ev = IO_EVENT_EPOLLOUT;
					}

					/* �������� */
					int dsr = postSend();
					if(dsr != IO_ERROR_SUCESS)
					{
						/* ��������ʱ����������ش���,����ERR�¼�, ERR�¼���EPOLLOUT���� */
						/* �Ѿ�������shutdown���,���һ������ڵ������Ѿ�ȫ������,�򴥷�ERROR�¼�ʹ�û����Եõ�֪ͨ */
						setLastError(dsr);
						ev = IO_EVENT_EPOLLERR;
					}
				}
				else
				{
					/* ����error�¼� */
					setLastError(IO_ERROR_HUP);
					ev = IO_EVENT_EPOLLHUP;
				}
			}
			break;
		default: break;
		}
	}

	return ev;
}

/*
To assure that all data is sent and received on a connected socket before it is closed, an application should use shutdown to close connection before calling closesocket. 
For example, to initiate a graceful disconnect:

	Call WSAAsyncSelect to register for FD_CLOSE notification. 
	Call shutdown with how=SD_SEND. 
	When FD_CLOSE received, call recv until zero returned, or SOCKET_ERROR. 
	Call closesocket. 
	Note  The shutdown function does not block regardless of the SO_LINGER setting on the socket.

	An application should not rely on being able to reuse a socket after it has been shut down. In particular, a Windows Sockets provider is not required to support the 
	use of connect on a socket that has been shut down.

* shutdown ����: ������ͻ����ڻ���������ȷ��ͻ�����պ󴥷�һ�� ERR �¼�; ������ͻ�����û���������ֶ�����.
*/
int IOCPContext::shutdown(int how)
{
	/*
	* ��������һ��shutdown���,��ֹ������recv/send����,Ȼ��
	* 1. ������ͻ����ڻ�������,��ȴ����ͻ��巢����Ϻ󴥷�һ��IO_EVENT_ERROR�¼�
	* 2. ������ͻ����ǿյ�,�򷵻�IO_ERROR_SUCESS��ʾ�û�����ֱ�ӵ���close()��ȫ�رն����õȴ��¼�֪ͨ.
	* -- ��û��RECV IO�ڽ����޹ؽ�Ҫ,�û�������Ľ��ջ����ڵ����ݿ���shutdown���غ����recv�õ�.
	*/
	_shutdownFlag = how;

	// �жϷ��ͻ����Ƿ�Ϊ��,����û��IO�������ڽ���.
	if(_sendOlp.ipos == 0 && _sendOlp.upos == 0)
	{
		assert(_sendOlp.oppType == IO_OPP_NONE);

		/* ��¼һ���¼�,�´���ӵ�selectorʱ���� */
		setLastError(IO_ERROR_SHUTDOWN);
	}
	else
	{
		/* ���ͻ��巢����Ϻ��Զ�����һ�� ERR �¼� */
		assert(_sendOlp.oppType == IO_OPP_SEND);
	}
	return IO_ERROR_SUCESS;
}

SOCKET IOCPContext::attach(SOCKET s)
{
	SOCKET oldSock = _sock;
	_sock = s;
	return oldSock;
}

SOCKET IOCPContext::detach()
{
	return attach(INVALID_SOCKET);
}

int IOCPContext::postAccept()
{
	assert(_sockType != SOCK_FILE);
	/* ��IO�������ڽ��л����Ѿ������һ�����ӵ������ӵ��׽���û�б�ȡ�� */
	if(_recvOlp.oppType != IO_OPP_NONE || _newSock != INVALID_SOCKET)
	{
		return IO_ERROR_SUCESS;
	}

	if(!_acceptBuf)
	{
		_acceptBuf = new char[MIN_SOCKADDR_BUF_SIZE * 2];
	}

	if(!_lpfnAcceptEx)
	{
		/*
		* ���AcceptEx()����ָ��
		*/
		GUID GuidAcceptEx = WSAID_ACCEPTEX;
		DWORD dwBytes = 0;
		if( 0 != WSAIoctl(_sock, SIO_GET_EXTENSION_FUNCTION_POINTER, &GuidAcceptEx, sizeof(GuidAcceptEx), &_lpfnAcceptEx, sizeof(_lpfnAcceptEx), &dwBytes, NULL, NULL) )
		{
			assert(0);
			return IO_ERROR_ACCEPTFAILED;
		}
	}

	/*
	* ����һ���µ��׽���(accept ���õ�ֻ����TCP���׽���)
	*/
	_newSock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	// ����һ�� IOCP AcceptEx ����
	DWORD dwBytesReceived = 0;
	_recvOlp.oppType = IO_OPP_ACCEPT;
	if(!_lpfnAcceptEx(_sock, _newSock, _acceptBuf, 0, MIN_SOCKADDR_BUF_SIZE, MIN_SOCKADDR_BUF_SIZE, &dwBytesReceived, (OVERLAPPED*)&_recvOlp))
	{
		if(WSA_IO_PENDING != WSAGetLastError() && WSAECONNRESET != WSAGetLastError())
		{
			return IO_ERROR_ACCEPTFAILED;
		}
	}
	return IO_ERROR_SUCESS;
}

int IOCPContext::postConnect(const char *ipAddr, u_short port)
{
	assert(_sockType != SOCK_FILE);
	/*
	* ȡ��ConnectEx����ָ��
	*/
	DWORD dwBytesReceived = 0;
	GUID GuidConnectEx = WSAID_CONNECTEX;
	LPFN_CONNECTEX lpfnConnectEx = NULL;
	DWORD dwBytes = 0;
	if( 0 != WSAIoctl(_sock, SIO_GET_EXTENSION_FUNCTION_POINTER, &GuidConnectEx, sizeof(GuidConnectEx), &lpfnConnectEx, sizeof(lpfnConnectEx), &dwBytes, NULL, NULL) )
	{
		assert(0);
		return IO_ERROR_CONNECTFAILED;
	}

	/*
	* ִ��ConnectEx
	*/
	_sendOlp.oppType = IO_OPP_CONNECT;
	
	sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = port;
	addr.sin_addr.S_un.S_addr = inet_addr(ipAddr);
	if( !lpfnConnectEx(_sock, (const sockaddr*)&addr, sizeof(sockaddr), NULL, 0, NULL, (LPOVERLAPPED)&_sendOlp) )
	{
		if(WSA_IO_PENDING != WSAGetLastError())
		{
			return IO_ERROR_CONNECTFAILED;
		}
	}

	return IO_ERROR_SUCESS;
}
/*
* recv io �ύ�Ļ��������Ǵ� [ipos -> end), ����������Ч���ݵķ�Χ�� [upos -> ipos) ��Ч����(�Ѿ���user��ȡ)��Χ�� [start, upos)
*/
int IOCPContext::postRecv()
{
	/* ������ shutdown ���,ֱ�ӷ��سɹ� */
	if(_shutdownFlag == SD_RECEIVE || _shutdownFlag == SD_BOTH)
	{
		return IO_ERROR_SUCESS;
	}

	/* ��IO�������ڽ��� */
	if(_recvOlp.oppType != IO_OPP_NONE)
	{
		assert(_recvOlp.oppType == IO_OPP_RECV);
		return IO_ERROR_SUCESS;
	}

	/*
	* ������ջ�����
	*/
	if(!_recvOlp.buf)
	{
		_recvOlp.buf = new byte[_recvOlp.len];
		_recvOlp.ipos = 0;
		_recvOlp.upos = 0;
	}

	/*
	* ���û�����,�����Ѿ���ȡ�Ĳ���
	*/
	memmove(_recvOlp.buf, _recvOlp.buf + _recvOlp.upos, _recvOlp.ipos - _recvOlp.upos);
	_recvOlp.ipos -= _recvOlp.upos;
	_recvOlp.upos = 0;
	
	if(_recvOlp.ipos >= _recvOlp.len)
	{
	}
	else
	{
		/*
		* Ͷ��һ��IO RECV ����
		*/
		_recvOlp.oppType = IO_OPP_RECV;
		WSABUF wsaBuf = { _recvOlp.len - _recvOlp.ipos, (char*)_recvOlp.buf + _recvOlp.ipos };
		DWORD dwFlags = 0;
		DWORD dwTransfered = 0;

		if(_sockType != SOCK_FILE)
		{
			if(SOCKET_ERROR == WSARecv(_sock, &wsaBuf, 1, &dwTransfered, &dwFlags, (LPOVERLAPPED)&_recvOlp, NULL))
			{
				if(WSAGetLastError() != WSA_IO_PENDING)
				{
					return IO_ERROR_RECVFAILED;
				}
			}
		}
		else
		{
			if(!ReadFile((HANDLE)_sock, wsaBuf.buf, wsaBuf.len, &dwTransfered, (LPOVERLAPPED)&_recvOlp))
			{
				if(GetLastError() != ERROR_IO_PENDING)
				{
					return IO_ERROR_RECVFAILED;
				}
			}
		}
	}
	return IO_ERROR_SUCESS;
}

/*
* send io �������ֲ�, �ύ���� [start -> ipos), ��дδ�ύ���� [ipos -> upos), ���ಿ�� [upos -> end)
*/
int IOCPContext::postSend()
{
	/* ��������˹رձ�־ҲҪ���������Ѿ��������������ڵ����� */
	/* ��IO�������ڽ��� */
	if(_sendOlp.oppType != IO_OPP_NONE)
	{
		assert(IO_OPP_SEND == _sendOlp.oppType);
		return IO_ERROR_SUCESS;
	}

	/* ���÷��ͻ�����,���Ѿ����ͳɹ������ݶ��� */
	assert(_sendOlp.buf);
	memmove(_sendOlp.buf, _sendOlp.buf + _sendOlp.ipos, _sendOlp.upos - _sendOlp.ipos);
	_sendOlp.upos -= _sendOlp.ipos;
	_sendOlp.ipos = _sendOlp.upos;

	if(_sendOlp.ipos <= 0)
	{
		/* ���������shutdown���,�����ͻ�����ȫ��������Ϻ󷵻� SHUTDOWN */
		if(_shutdownFlag == SD_BOTH || _shutdownFlag == SD_SEND)
		{
			return IO_ERROR_SHUTDOWN;
		}
		else
		{
			return IO_ERROR_SUCESS;
		}
	}
	else
	{
		/*
		* Ͷ��һ��IO SEND ����
		*/	
		_sendOlp.oppType = IO_OPP_SEND;
		DWORD dwTransfered = 0;
		DWORD dwLastError = 0;
		WSABUF wsaBuf = { _sendOlp.ipos, (char*)_sendOlp.buf };

		if(_sockType != SOCK_FILE)
		{
			if(SOCKET_ERROR == WSASend(_sock, &wsaBuf, 1, &dwTransfered, 0, (LPOVERLAPPED)&_sendOlp, NULL))
			{
				if(WSAGetLastError() != WSA_IO_PENDING)
				{
					return IO_ERROR_SENDFAILED;
				}
			}
		}
		else
		{
			if(!WriteFile((HANDLE)_sock, wsaBuf.buf, wsaBuf.len, &dwTransfered, (LPOVERLAPPED)&_sendOlp))
			{
				if(GetLastError() != ERROR_IO_PENDING)
				{
					return IO_ERROR_SENDFAILED;
				}
			}
		}

		return IO_ERROR_SUCESS;
	}
}

SOCKET IOCPContext::accept()
{
	assert(_sockType != SOCK_FILE);
	SOCKET newSock = INVALID_SOCKET;

	lock();
	if(_recvOlp.oppType == IO_OPP_NONE)
	{
		// AccpetEx�����Ѿ����,�������ӷ���
		newSock = _newSock;
		_newSock = INVALID_SOCKET;

		// ������һ��IO
		setLastError(postAccept());
	}
	else
	{
	}
	unlock();

	return newSock;
}

int IOCPContext::connect(const char* ipAddr, u_short port)
{
	assert(_sockType != SOCK_FILE);

	/* ����,��ֹ�û��ظ����� */
	int ret = IO_ERROR_UNDEFINED;
	lock();
	ret = postConnect(ipAddr, port);
	unlock();
	return ret;
}

int IOCPContext::recv(void* buf, size_t len)
{
	// ���ƻ������ڵ�����
	lock();
	if(len > _recvOlp.ipos - _recvOlp.upos) len = _recvOlp.ipos - _recvOlp.upos;
	if(len > 0)
	{
		if(buf)
		{
			memcpy(buf, _recvOlp.buf + _recvOlp.upos, len);
		}
		_recvOlp.upos += len;
	}
	else
	{
		/* ��Ե������� */
		_recvOlp.et = true;
	}

	// ����һ��RECV IO
	setLastError(postRecv());
	unlock();

	return len;
}

int IOCPContext::send(const void* buf, size_t len)
{
	/* ����Ѿ�������shutdown���,��������д���� */
	if(SD_BOTH == _shutdownFlag || SD_SEND == _shutdownFlag)
	{
		return IO_ERROR_SHUTDOWN;
	}

	lock();

	/* ���䷢�ͻ���  */
	if(!_sendOlp.buf)
	{
		_sendOlp.buf = new byte[_sendOlp.len];
		_sendOlp.ipos = 0;
		_sendOlp.upos = 0;
	}

	// д�뻺����
	if(len > _sendOlp.len - _sendOlp.upos) len = _sendOlp.len - _sendOlp.upos;
	memcpy(_sendOlp.buf + _sendOlp.upos, buf, len);
	_sendOlp.upos += len;

	if(_sendOlp.upos >= _sendOlp.len)
	{
		_sendOlp.et = true;
	}

	// ����һ�� send IO
	setLastError(postSend());

	unlock();
	return len;
}

// ���ض��������д����ĳ���
size_t IOCPContext::size(bool r)
{
	size_t s = 0;
	lock();
	if(r)
	{
		s =  _recvOlp.ipos - _recvOlp.upos;
	}
	else
	{
		s = _sendOlp.len - _sendOlp.upos;
	}
	unlock();
	return s;
}

size_t IOCPContext::_pump(reader_t reader, size_t maxlen)
{
	/* ����Ѿ�������shutdown���,��������д���� */
	if(SD_BOTH == _shutdownFlag || SD_SEND == _shutdownFlag)
	{
		return 0;
	}

	lock();

	/* ���䷢�ͻ���  */
	if(!_sendOlp.buf)
	{
		_sendOlp.buf = new byte[_sendOlp.len];
		_sendOlp.ipos = 0;
		_sendOlp.upos = 0;
	}

	// д�뻺����
	size_t len = maxlen;
	if(len > _sendOlp.len - _sendOlp.upos) len = _sendOlp.len - _sendOlp.upos;
	len = reader(_sendOlp.buf + _sendOlp.upos, len);
	_sendOlp.upos += len;

	if(_sendOlp.upos >= _sendOlp.len)
	{
		_sendOlp.et = true;
	}

	// ����һ�� send IO
	setLastError(postSend());

	unlock();
	return len;
}