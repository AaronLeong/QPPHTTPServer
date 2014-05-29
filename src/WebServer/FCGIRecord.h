/* Copyright (C) 2011 ������
 *
 * ����һ����Դ������,���������ɵ��޸ĺͷ���.
 * ��ֹ������ҵ��;.
 *
 * ��ϵԭ����: querw@sina.com 
*/

#pragma once
#include <list>
#include "fastcgi.h"
#include "Buffer.h"
#include "Pipes/Pipe.h"

/*
* ��װFastCGI Record, FastCGI Record �� FastCGI Э��Ļ���ͨѶ��λ.
* һ�� Record �Ľṹ����:
	typedef struct {
		unsigned char version;
		unsigned char type;
		unsigned char requestIdB1;
		unsigned char requestIdB0;
		unsigned char contentLengthB1;
		unsigned char contentLengthB0;
		unsigned char paddingLength;
		unsigned char reserved;

		unsigned char contentData[contentLength];
		unsigned char paddingData[paddingLength];
	} FCGI_Record;
* ǰ8���ֽ��� FCGI_Header �ṹ, ����������� FCGI_Header ָ�������ݳ��� contentLength �� ���볤�� paddingLength ָ��.
* ������Ϊ����һ�α䳤���ڴ滺��.
* һЩ�ض����͵� Record contentData ���ض���Ԥ����ṹ,�� FCGI_BeginRequestBody, FCGI_EndRequestBody �� FCGI_UnknownTypeBody.
*
* class FCGIRecord ��Ŀ���Ƿ�װFastCGI record�Է����̷���. ���� reader �� writer �ֱ����ڽ��������� FastCGI record.
*/

/* 
* Name - Value pair 
*/
typedef struct 
{ 
	const unsigned char* data; 
	size_t len; 
}nv_t;
typedef std::pair<nv_t, nv_t> nvpair_t;
typedef std::list<nvpair_t> nvlist_t;

/*
* FCGI Record wrapper
*/
class FCGIRecord
{
private:
	Buffer _buffer; /* record ԭʼ���� */
	
public:
	FCGIRecord();
	FCGIRecord(const FCGIRecord& rh);
	FCGIRecord(const void* buf, size_t len);
	~FCGIRecord();

	FCGIRecord& operator = (const FCGIRecord& rh);
	size_t assign(const void* buf, size_t len);
	size_t assign(const Buffer* buf);
	size_t assign(IPipe* p);
	const void* buffer() const;
	size_t size() const;

	void reset();
	bool check(); /* ����Ƿ���һ�������� record */

	bool setHeader(unsigned short requestId, unsigned char type);
	bool setBeginRequestBody(unsigned short role, bool keepConn); /* FCGI_BEGIN_REQUEST */
	bool setEndRequestBody(unsigned int appStatus, unsigned char protocolStatus); /* FCGI_END_REQUEST */
	bool setUnknownTypeBody(); /* FCGI_UNKNOWN_TYPE */
	bool addNameValuePair(nv_t n, nv_t v); /* FCGI_PARAMS,FCGI_GET_VALUES,FCGI_GET_VALUES_RESULT */
	bool addNameValuePair(const char* n, const char* v);
	size_t addBodyData(const void* buf, size_t len); /* ͨ��,FCGI_STDIN,FCGI_STDOUT,FCGI_STDERR,FCGI_DATA*/
	bool setEnd(unsigned char padding = 0); /* ��� */

	bool getHeader(FCGI_Header &header);
	unsigned char getType();
	size_t getContentLength();
	bool getBeginRequestBody(unsigned short &role, bool &keepConn); /* FCGI_BEGIN_REQUEST */
	bool getEndRequestBody(unsigned int &appStatus, unsigned char &protocolStatus); /* FCGI_END_REQUEST */
	size_t getNameValuePairCount();
	bool getNameValuePair(int index, nv_t &n, nv_t &v); /* FCGI_PARAMS,FCGI_GET_VALUES,FCGI_GET_VALUES_RESULT */
	const void* getBodyData(); /* ͨ��,FCGI_STDIN,FCGI_STDOUT,FCGI_STDERR,FCGI_DATA*/
	size_t getBodyLength();
};