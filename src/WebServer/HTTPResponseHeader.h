/* Copyright (C) 2012 ������
 *
 * ����һ����Դ������,���������ɵ��޸ĺͷ���.
 * ��ֹ������ҵ��;.
 *
 * ��ϵԭ����: querw@sina.com 
*/


#pragma once
#include <string>
#include <list>
#include "Buffer.h"
#include "HTTPLib.h"
#include "Pipes/BufferPipe.h"

/*
* HTTPResponseHeader �������� HTTP ��Ӧͷ
*/

class HTTPResponseHeader : public IPipe
{
private:
	typedef std::list<std::pair<std::string, std::string>> fields_t;
	fields_t _headers;	// ��Ӧͷ�Ĺ�������
	int _resCode; // ��Ӧ��
	BufferPipe _buffer;

	std::string getFirstLine(); // �����ƶ��� Servercode ���ض�Ӧ�� HTTP��Ӧͷ�ĵ�һ��,�������з�.
	size_t puts(const std::string &str);
	fields_t::iterator find(const std::string &name);

	size_t _read(void* buf, size_t len);
	size_t _pump(reader_t reader, size_t maxlen);
	__int64 _size();

public:
	HTTPResponseHeader();
	~HTTPResponseHeader();

	int setResponseCode(int resCode); // ������Ӧ��,���ؾ�ֵ
	int getResponseCode();
	size_t addDefaultFields(); // ���Ĭ����
	bool add(const std::string &name, const std::string &val); // �����
	bool add(const std::string &fields); // �����,����Ϊ��λ,һ�ο���Ӷ����
	bool remove(const std::string &name); // ɾ����
	bool getField(const std::string &name, std::string &val);
	std::string getHeader();
	bool format(); // ��ʽ��������
	void reset();
};

