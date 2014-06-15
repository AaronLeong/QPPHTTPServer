/* Copyright (C) 2011 ������
 *
 * ����һ����Դ������,���������ɵ��޸ĺͷ���.
 * ��ֹ������ҵ��;.
 *
 * ��ϵԭ����: querw@sina.com 
*/

#include "StdAfx.h"
#include "HTTPRequest.h"

HTTPRequest::HTTPRequest(IHTTPServer *server) 
	: _httpHeader(NULL), _cachePipe(NULL), _httpPostData(NULL), _server(server), _contentLength(0), _bytesRecved(0), _bytesSent(0)
{
	_cachePipe = new BufferPipe(MAX_REQUESTHEADERSIZE, K_BYTES);
}

HTTPRequest::~HTTPRequest()
{
	if(_cachePipe) delete _cachePipe;
	if(_httpHeader) delete _httpHeader;
	if(_httpPostData) delete _httpPostData;
}

bool HTTPRequest::isValid()
{
	if(_httpHeader)
	{
		// ����Ƿ��Ѿ��յ�������������ͷ
		const char* header = NULL;
		size_t headerLen = 0;
		getHeaderBuffer(&header, &headerLen);
		
		if(http_header_end(header, headerLen) > 0)
		{
			if(contentLength() == 0 || (_httpPostData && _httpPostData->size() == contentLength()))
			{
				return true;
			}
			else
			{
				// content length ����ȷ.
			}
		}
		else
		{
			// ����ͷ������.
		}
	}
	else
	{
		// û�н��յ�����ͷ.
	}

	return false;
}

void HTTPRequest::getHeaderBuffer(const char** buf, size_t *len)
{
	if(_httpHeader)
	{
		*buf = (const char*)_httpHeader->buffer();
		*len = (size_t)_httpHeader->size() - 1;
	}
	else
	{
		*buf = NULL;
		*len = 0;
	}
}

bool HTTPRequest::keepAlive()
{
	return stricmp(field("Connection").c_str(), "keep-alive") == 0;
}

size_t HTTPRequest::contentLength()
{
	return _contentLength;
}

HTTP_METHOD HTTPRequest::method()
{
	const char* header = NULL;
	size_t headerLen = 0;
	getHeaderBuffer(&header, &headerLen);

	// ȡ�� HTTP ����
	char szMethod[MAX_METHODSIZE] = {0};
	int nMethodIndex = 0;
	for(size_t i = 0; i < MAX_METHODSIZE && i < headerLen; ++i)
	{
		if(header[i] != ' ')
		{
			szMethod[nMethodIndex++] = header[i];
		}
		else
		{
			break;
		}
	}

	// ����
	if( strcmp(szMethod, "GET") == 0 ) return METHOD_GET;
	if( strcmp(szMethod, "PUT") == 0 ) return METHOD_PUT;
	if( strcmp(szMethod, "POST") == 0 ) return METHOD_POST;
	if( strcmp(szMethod, "HEAD") == 0 ) return METHOD_HEAD;
	if( strcmp(szMethod, "DELETE") == 0 ) return METHOD_DELETE; // ɾ��
	if( strcmp(szMethod, "TRACE") == 0 ) return METHOD_TRACE;
	if( strcmp(szMethod, "CONNECT") == 0 ) return METHOD_CONNECT;

	return METHOD_UNKNOWN;
}

// ���ؿͻ����������, ������ؿ��ַ���,˵���ͻ��������ʽ����.
std::string HTTPRequest::uri(bool decode)
{
	const char* header = NULL;
	size_t headerLen = 0;
	getHeaderBuffer(&header, &headerLen);

	std::string strObject("");
	const char* lpszRequest = header;
	const char *pStart = NULL, *pEnd = NULL;

	// ��һ�еĵ�һ���ո����һ���ַ���ʼ��������ļ�����ʼ.
	for(size_t i = 0; i < headerLen; ++i)
	{
		if(lpszRequest[i] == ' ')
		{
			pStart = lpszRequest + i + 1; 
			break;
		}
		if(lpszRequest[i] == '\n') break;
	}
	if(pStart == NULL)
	{
		// �Ҳ�����ʼλ��
		return strObject;
	}

	// �ӵ�һ�е�ĩβ������ҵ�һ���ո�,ʵ��: GET / HTTP/1.1
	pEnd = strstr(lpszRequest, "\r\n"); 
	if(pEnd == NULL || pEnd < pStart) 
	{
		/* �Ҳ�����βλ�� */
		assert(0);
		return strObject;
	}

	// �ѽ�β�Ŀո��Ƴ�
	while(pEnd >= pStart)
	{
		if(pEnd[0] == ' ')
		{
			pEnd--;
			break;
		}
		pEnd--;
	}

	if(pEnd == NULL || pEnd < pStart)
	{
		assert(0);
	}
	else
	{
		strObject.assign(pStart, pEnd - pStart + 1);
	}

	if(decode) return decode_url(strObject);
	else return strObject;
}

std::string HTTPRequest::field(const char* pszKey)
{
	const char* header = NULL;
	size_t headerLen = 0;
	getHeaderBuffer(&header, &headerLen);

	return get_field(header, pszKey);
}

bool HTTPRequest::range(__int64 &lFrom, __int64 &lTo)
{
	__int64 nFrom = 0;
	__int64 nTo = -1; // -1 ��ʾ�����һ���ֽ�.

	const char* header = NULL;
	size_t headerLen = 0;
	getHeaderBuffer(&header, &headerLen);

	const char* lpszRequest = header;
	const char* pRange = strstr(lpszRequest, "\r\nRange: bytes=");
	if(pRange)
	{
		/*
		The first 500 bytes (byte offsets 0-499, inclusive):
		bytes=0-499
		The second 500 bytes (byte offsets 500-999, inclusive):
		bytes=500-999
		The final 500 bytes (byte offsets 9500-9999, inclusive):
		bytes=-500
		bytes=9500-
		The first and last bytes only (bytes 0 and 9999):
		bytes=0-0,-1
		Several legal but not canonical specifications of the second 500 bytes (byte offsets 500-999, inclusive):
		bytes=500-600,601-999
		bytes=500-700,601-999
		*/

		pRange += strlen("\r\nRange: bytes=");
		const char *pMinus = strchr(pRange, '-');
		if(pMinus)
		{
			char szFrom[200], szTo[200];
			memset(szFrom, 0, 200);
			memset(szTo, 0, 200);
			memcpy(szFrom, pRange, pMinus - pRange);
			nFrom = _atoi64(szFrom);

			pMinus++;
			pRange = strstr(pMinus, "\r\n");
			if(pMinus + 1 == pRange)
			{
				nTo = -1;
			}
			else
			{
				memcpy(szTo, pMinus, pRange - pMinus);
				nTo = _atoi64(szTo);
				if(nTo <= 0) nTo = -1;
			}

			lFrom = nFrom;
			lTo = nTo;

			return true;
		}
		else
		{
		}
	}
	else
	{
	}
	return false;
}

std::string HTTPRequest::getHeader()
{
	const char* header = NULL;
	size_t headerLen = 0;
	getHeaderBuffer(&header, &headerLen);
	
	return header;
}

IPipe* HTTPRequest::getPostData()
{
	return _httpPostData;
}

void HTTPRequest::nextRequest()
{
	if(_httpHeader) delete _httpHeader;
	if(_httpPostData) delete _httpPostData;
	_httpHeader = NULL;
	_httpPostData = NULL;
	_contentLength = 0;
	_bytesRecved = 0;
	_bytesSent = 0;

	// ״̬��״̬����
	backward(current());
}

bool HTTPRequest::beforeStep(IOAdapter* adp, int ev, stm_result_t* res)
{
	if(TEST_BIT(ev, IO_EVENT_EPOLLERR))
	{
		/* �׽��ִ��� */
		res->rc = STM_ABORT;
		res->param.errorCode = SE_RECVFAILD;
		return false;
	}
	if(TEST_BIT(ev, IO_EVENT_EPOLLHUP))
	{
		/* �Է��Ѿ��ر������� */
		res->rc = STM_ABORT;
		res->param.errorCode = SE_REMOTEFAILD;
		return false;
	}

	return true;
}

bool HTTPRequest::step0(IOAdapter* adp, int ev, stm_result_t* res)
{
	assert(_cachePipe);

	/* ����һ������ͷ�Ľ��մ���,������������ͷ��POSTdata */
	build_pipes_chain((IPipe*)adp, (IPipe*)_cachePipe);
	forward();

	res->rc = STM_CONTINUE;
	res->param.epollEvent = IO_EVENT_EPOLLIN;
	return false;
}

bool HTTPRequest::step1(IOAdapter* adp, int ev, stm_result_t* res)
{
	// ��ȡ�׽�������
	size_t rd = _cachePipe->pump();
	_bytesRecved += rd;
	if(0 == rd)
	{
		// ����ͷ���ȳ�������
		assert(0);
		forward(2);
		return true;
	}
	else
	{
		// �ж��Ƿ��Ѿ���ȡ������������ͷ
		int headerlen = http_header_end((const char*)_cachePipe->buffer(), (size_t)_cachePipe->size());
		if(headerlen < 0)
		{
			// ������������ͷ
		}
		else
		{
			// ������ͷ�ӻ����ж�����������.֮�������һ��������Ϊ�˴���HTTP1.1Э���"�ܵ�������"(�ͻ������յ���һ����Ӧ֮ǰ���������������ɸ�����ͷ)
			// ���������������δ���.
			assert(_httpHeader == NULL);
			_httpHeader = new BufferPipe(MAX_REQUESTHEADERSIZE, K_BYTES);
			size_t plen = _httpHeader->pump(headerlen, _cachePipe);
			assert(plen == headerlen);

			 // ����ͷ���ַ���,д��0��β.
			char c = 0;
			_httpHeader->write(&c, 1);

			// ����Content-Length��ֵ,֮����Է��� contentLength() ֱ�ӻ�ȡ,������������ͷ�ı��ڲ���.
			_contentLength = atoi(field("Content-Length").c_str());
			if(contentLength() >= MAX_POST_DATA)
			{
				/* content-length ���ȳ������� */
				forward(2);
				return true;
			}
			else if(contentLength() > 0)
			{
				// ������ȳ��� POST_DATA_CACHE_SIZE ������ʱ�ļ�,����ֱ�����ڴ��л���.
				if(contentLength() > POST_DATA_CACHE_SIZE)
				{
					_httpPostData = new FilePipe(_server->tmpFileName());
				}
				else
				{
					_httpPostData = new BufferPipe(POST_DATA_CACHE_SIZE, (4 * K_BYTES));
				}
				_httpPostData->link(_cachePipe);


				/* �� _cachePipe ��ȡPOST DATA */
				forward();
				return true;
			}
			else
			{
				// û��POST����
				forward(2);
				return true;
			}
		}// continue to receive header
	}// continue to pump

	// ������������ͷ
	res->rc = STM_CONTINUE;
	res->param.epollEvent = IO_EVENT_EPOLLIN;
	return false;
}

bool HTTPRequest::step2(IOAdapter* adp, int ev, stm_result_t* res)
{
	assert(_httpPostData);
	size_t maxlen = contentLength() - (size_t)_httpPostData->size();
	size_t rd = 0;

	// ����post����
	if(_cachePipe->size() > 0)
	{
		// �����Ѿ��ȶ�ȡ��������,��Ҫ�ظ�ͳ��.
		rd = _httpPostData->pump(maxlen);
	}
	else
	{
		// ֱ�Ӵ� IOAdapter �ж�ȡ
		rd = _httpPostData->pump(maxlen);
		_bytesRecved += rd;

		if(rd == 0)
		{
			// 400 Bad request
			forward();
			return true;
		}
	}

	if(_httpPostData->size() >= contentLength())
	{
		// POST DATA�������.
		_httpPostData->unlink();
		
		forward();
		return true;
	}
	else
	{
		// �������� POST ����
		res->rc = STM_CONTINUE;
		res->param.epollEvent = IO_EVENT_EPOLLIN;
		return false;
	}
}

bool HTTPRequest::step3(IOAdapter* adp, int ev, stm_result_t* res)
{
	res->rc = STM_DONE;
	return false;
}

void HTTPRequest::statistics(__int64* bytesRecved, __int64* bytesSent)
{
	*bytesRecved = _bytesRecved;
	*bytesSent = _bytesSent;
}