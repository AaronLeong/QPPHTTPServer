/* Copyright (C) 2012 ������
 *
 * ����һ����Դ������,���������ɵ��޸ĺͷ���.
 * ��ֹ������ҵ��;.
 *
 * ��ϵԭ����: querw@sina.com 
*/

#pragma once
#include <string>
#include <utility>
#include <vector>

#ifdef UNICODE
typedef std::wstring tstring;
typedef wchar_t tchar;
#define URI_TEXT(x) L ## x
#else
typedef std::string tstring;
typedef char tchar;
#define URI_TEXT(x) x
#endif

class Uri
{
public:

	// ���������Ͷ���
	typedef std::pair<tstring, tstring> param_t;
	typedef std::vector<param_t> param_list_t;

	// url Э������
	typedef enum _protocol_t
	{
		file = 1,
		gopher,
		http,
		https,
		ftp,
		mailto,
		mms,
		news,
		nntp,
		telnet,
		wais,
		prospero,

		unknown = 100
	}protocol_t;

private:
	// ԭʼ��url�ַ���
	protocol_t _defaultProtoType;
	protocol_t _protType;
	unsigned int _port;
	tstring _urlStr;
	tstring _hostname;
	tstring _path;
	tstring _parameters;
	tstring _content; // ����Э����֮���ȫ��
	tstring _username;
	tstring _password;
	
	param_list_t _httpParamList;// http �����б�
	
private:
	static protocol_t _mapProtocolType(const tstring& protoName);
	static unsigned int _getDefaultPort(protocol_t protoType);
	static tstring _getSubStr(const tstring& str, tstring::const_iterator& first, tstring::const_iterator& second);
	bool _parseUrl(const tstring& urlStr);
	void _reset();

public:
	// ���캯��
	Uri(protocol_t defaultProtoType = http);
	Uri(const tstring& urlStr, protocol_t defaultProtoType = http);
	Uri(const Uri& rh);
	~Uri(void);

	// ��̬����,URL����ͽ��� (UTF8)
	static tstring encode(const tstring& inputStr);
	static tstring decode(const tstring& inputStr);

	// �Ƿ���һ����Ч��url
	bool isValid();

	// ����Э��ͨ�õ�,����url�ĸ�������
	protocol_t protocol();
	unsigned int port();
	const tstring& uri();
	const tstring& content();
	const tstring& username();
	const tstring& password();
	const tstring& hostname();
	const tstring& path();
	const tstring& parameters();

	// ��Э��ר�ú���
	const param_list_t& httpParamList();
	bool httpQuery(const tstring& paramKey, tstring& paramValue);

	// ����url
	Uri& operator = (const Uri& rh);
	Uri& operator = (const tstring& urlStr);
	bool operator == (const Uri& rh);

	// ʹ�������﷨�������� if (url) ���� if(!url)
	bool operator !();
	operator void * ();
};

