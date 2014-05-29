/* Copyright (C) 2011 ������
 *
 * ����һ����Դ������,���������ɵ��޸ĺͷ���.
 * ��ֹ������ҵ��;.
 *
 * ��ϵԭ����: querw@sina.com 
*/

#pragma once
#include "HTTPLib.h"

/*
* HTTPContent
* ������װ HTTP ��Ӧ�����е����ݲ���,�п��ܰ������¼�����������:
* 1. һ���ı�,�� HTTP 500 �ȴ�����ʾ.
* 2. һ��Ŀ¼�ı���,���Ŀ¼ʱ�õ����ļ��б�.
* 3. һ��ֻ���ļ��Ĳ��ֻ���ȫ��.
*
*/

#define CONTENT_TYPE_FILE 1
#define CONTENT_TYPE_TEXT 2
#define CONTENT_TYPE_BINARY 3
#define CONTENT_TYPE_HTML 4

class HTTPContent : public IPipe
{
private:
	IPipe *_content;
	std::string _contentType;
	std::string _contentRange;
	std::string _etag;
	std::string _lastModify;
	bool _acceptRanges;
	__int64 _contentLength;

	size_t puts(const char* str);

protected:
	// from pipe
	size_t _read(void* buf, size_t len);
	size_t _pump(reader_t reader, size_t maxlen);
	__int64 _size();

public:
	HTTPContent();
	virtual ~HTTPContent();
	bool open(const std::string &fileName, __int64 from = 0, __int64 to = 0); /* ��һ��ֻ���ļ� */
	bool open(const std::string &urlStr, const std::string &filePath); /* ��һ��Ŀ¼ */
	bool open(const char* buf, size_t len, int type); /* ��һ�� mem buffer */
	void close();

	std::string& contentType();
	std::string& contentRange();
	__int64 contentLength();
	std::string& lastModified();
	std::string& etag();
	bool acceptRanges();
};
