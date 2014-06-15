/* Copyright (C) 2011 ������
 *
 * ����һ����Դ������,���������ɵ��޸ĺͷ���.
 * ��ֹ������ҵ��;.
 *
 * ��ϵԭ����: querw@sina.com 
*/

#include "StdAfx.h"
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>

#include "HTTPLib.h"
#include "HTTPContent.h"
#include "Pipes/FilePipe.h"
#include "Pipes/BufferPipe.h"

static void get_content_type_from_filename(std::string& strType, const char* pszFileName)
{
	strType = "application/octet-stream";

	const char *pExt = strrchr(pszFileName, '.');
	if(pExt && strlen(pExt) < 1024)
	{
		char szExt[1025];
		strcpy(szExt, pExt + 1);

		if(stricmp(szExt, "jpg") == 0)
		{
			strType =  "image/jpeg";
		}
		else if(stricmp(szExt, "txt") == 0)
		{
			strType = "text/plain";
		}
		else if(stricmp(szExt, "htm") == 0)
		{
			strType = "text/html";
		}
		else if(stricmp(szExt, "html") == 0)
		{
			strType = "text/html";
		}
		else if(stricmp(szExt, "gif") == 0)
		{
			strType = "image/gif";
		}
		else if(stricmp(szExt, "png") == 0)
		{
			strType = "image/png";
		}
		else if(stricmp(szExt, "bmp") == 0)
		{
			strType = "image/x-xbitmap";
		}
		else
		{
		}
	}
}

static void map_content_type_string(std::string& strType, int t)
{
	strType = "application/octet-stream";

	if(t == CONTENT_TYPE_BINARY)
	{
	}
	else if(CONTENT_TYPE_TEXT == t)
	{
		strType = "text/plain";
	}
	else if(CONTENT_TYPE_HTML == t)
	{
		strType = "text/html";
	}
	else
	{
	}
}

static void format_range(std::string& str, __int64 fr, __int64 to, __int64 sz)
{
	char szRanges[300] = {0};
	if(fr < 0) fr = 0;
	if(fr >= sz) fr = sz - 1;
	if(to < 0) to = sz - 1;
	if(to >= sz) to = sz - 1;
	sprintf(szRanges, "bytes %lld-%lld/%lld", fr, to, sz);
	str = szRanges;
}

static void build_etag_string(std::string& str, const void* buf, size_t len)
{
	char szLength[201] = {0};
	ltoa(len, szLength, 10);

	// ������ڴ�����, ���ݴ�С��ȡ���ɸ��ֽڵ�16�����ַ�����.
	unsigned char szValue[ETAG_BYTE_SIZE + 1];
	for(int i = 0; i < ETAG_BYTE_SIZE; ++i)
	{
		szValue[i] = ((const char*)buf)[(len / ETAG_BYTE_SIZE) * i];
	}

	str = to_hex(szValue, ETAG_BYTE_SIZE);
	str += ":";
	str += szLength;
}

static void build_etag_string(std::string& str, struct _stat64 *fileInf)
{
	char szLength[201] = {0};
	_i64toa(fileInf->st_size, szLength, 10);

	// �����ļ���С���޸����ڴ���. [ETag: ec5ee54c00000000:754998030] �޸�ʱ���HEXֵ:�ļ�����
	// ȷ��ͬһ����Դ�� ETag ��ͬһ��ֵ.
	// ��ʹ�ͻ��������ֻ�������Դ��һ����.
	// �ϵ������ͻ��˸��� ETag ��ֵȷ�����صļ��������ǲ���ͬһ���ļ�.
	str = to_hex((const unsigned char*)&(fileInf->st_mtime), sizeof(fileInf->st_mtime));
	str += ":";
	str += szLength;
}

HTTPContent::HTTPContent()
	: _content(NULL), _contentLength(0), _acceptRanges(true)
{
}

HTTPContent::~HTTPContent()
{
	close();
}

bool HTTPContent::open(const std::string &fileName, __int64 from, __int64 to)
{
	assert(_content == NULL);
	struct _stat64 fileInf;
	if( 0 != _stat64(fileName.c_str(), &fileInf))
	{
		return false;
	}
	else
	{
		_content = new FilePipe(fileName, from, to);

		get_content_type_from_filename(_contentType, fileName.c_str());
		format_range(_contentRange, from, to, fileInf.st_size);
		build_etag_string(_etag, &fileInf);
		_contentLength = fileInf.st_size;
		_lastModify = format_http_date(&fileInf.st_mtime);
		_acceptRanges = true;
		
		return true;
	}
}

bool HTTPContent::open(const char* buf, size_t len, int type)
{
	assert(_content == NULL);
	__int64 ltime;
	_time64(&ltime);

	_content = new BufferPipe(len + 1, len);
	_content->write(buf, len);
	
	map_content_type_string(_contentType, type);
	format_range(_contentRange, 0, len - 1, len);
	build_etag_string(_etag, buf, len);
	_contentLength = len;
	_lastModify = format_http_date(&ltime);
	_acceptRanges = false;
	return true;
}

/*
* ��ȡĿ¼�б����Ϊһ��HTML�ı���.
*/
bool HTTPContent::open(const std::string &urlStr, const std::string &filePath)
{
	assert(_content == NULL);
	char buffer[_MAX_PATH + 100] = {0};
	char sizeBuf[_MAX_PATH + 100] = {0};

	// ��������,����һ��UTF8 HTML�ı���,�������ļ��б�.
	BufferPipe* bp = new BufferPipe(MAX_DIRECTORY_LIST_SIZE, 2 * K_BYTES);
	_content = bp;
	
	// 1. ���HTMLͷ,��ָ��UTF-8�����ʽ
	puts("<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\"/></head>");
	puts("<body>");

	// 2. ���·��
	//(1). �����һ�� ��Ŀ¼
	puts("<A href=\"/\">/</A>");

	//(2). ����Ŀ¼
	std::string::size_type st = 1;
	std::string::size_type stNext = 1;
	while( (stNext = urlStr.find('/', st)) != std::string::npos)
	{
		std::string strDirName =  urlStr.substr(st, stNext - st + 1);
		std::string strSubUrl = urlStr.substr(0, stNext + 1);

		puts("&nbsp;|&nbsp;");

		puts("<A href=\"");
		puts(AtoUTF8(strSubUrl).c_str());
		puts("\">");
		puts(AtoUTF8(strDirName).c_str());
		puts("</A>");
		
		// ��һ��Ŀ¼
		st = stNext + 1;
	}
	puts("<br /><hr />");

	// 3. �г���ǰĿ¼�µ������ļ�
	std::string strFullName;
	strFullName = filePath;
	if(strFullName.back() != '\\') strFullName += '\\'; // ���������'\\'��β���ļ�·��,����. ע��һ��ԭ��:URL����б�ָܷ�;�ļ����Է�б�ָܷ�
	strFullName += "*";

	std::string strFiles(""); // ��ͨ�ļ�д������ַ�����.

	__finddata64_t fileinfo;
	intptr_t findRet = _findfirst64(strFullName.c_str(), &fileinfo);
	if( -1 != findRet )
	{
		do 
		{
			// ���� . �ļ�
			if( stricmp(fileinfo.name, ".") == 0 || 0 == stricmp(fileinfo.name, "..") )
			{
				continue;
			}

			// ����ϵͳ�ļ��������ļ�
			if( fileinfo.attrib & _A_SYSTEM || fileinfo.attrib & _A_HIDDEN )
			{
				continue;
			}

			// �����Ŀ¼����
			if( fileinfo.attrib & _A_SUBDIR )
			{
				// �������Ŀ¼,ֱ��д��

				// ����޸�ʱ��
				_ctime64_s( buffer, _countof(buffer), &fileinfo.time_write );
				puts(AtoUTF8(buffer).c_str());

				// Ŀ¼����Ҫת��ΪUTF8����
				sprintf(buffer, "%s/", fileinfo.name);
				std::string fileurl = AtoUTF8(urlStr.c_str());
				std::string filename = AtoUTF8(buffer);

				puts("&nbsp;&nbsp;");
				puts("<A href=\"");
				puts(fileurl.c_str());
				puts(filename.c_str());
				puts("\">");
				puts(filename.c_str());
				puts("</A>");

				// д��Ŀ¼��־
				puts("&nbsp;&nbsp;[DIR]");

				// ����
				puts("<br />");
			}
			else
			{
				// ��ͨ�ļ�,д�뵽һ��������ַ���string������,ѭ�����ٺϲ�.����,���е�Ŀ¼����ǰ��,�ļ��ں���
				_ctime64_s( buffer, _countof(buffer), &fileinfo.time_write );
				strFiles += AtoUTF8(buffer);

				// �ļ���ת��ΪUTF8������д��
				std::string filename = AtoUTF8(fileinfo.name);
				std::string fileurl = AtoUTF8(urlStr.c_str());

				strFiles += "&nbsp;&nbsp;";
				strFiles += "<A href=\"";
				strFiles += fileurl;
				strFiles += filename;
				strFiles += "\">";
				strFiles += filename;
				strFiles += "</A>";

				// �ļ���С
				// ע: ����Windows�� wsprintf ��֧�� %f ����,����ֻ���� sprintf ��
				double filesize = 0;
				if( fileinfo.size >= G_BYTES)
				{
					filesize = (fileinfo.size * 1.0) / G_BYTES;
					sprintf(sizeBuf, "%.2f&nbsp;GB", filesize);
				}
				else if( fileinfo.size >= M_BYTES ) // MB
				{
					filesize = (fileinfo.size * 1.0) / M_BYTES;
					sprintf(sizeBuf, "%.2f&nbsp;MB", filesize);
				}
				else if( fileinfo.size >= K_BYTES ) //KB
				{
					filesize = (fileinfo.size * 1.0) / K_BYTES;
					sprintf(sizeBuf, "%.2f&nbsp;KB", filesize);
				}
				else // Bytes
				{
					sprintf(sizeBuf, "%lld&nbsp;Bytes", fileinfo.size);
				}
			
				strFiles += "&nbsp;&nbsp;";
				strFiles += sizeBuf;

				// ����
				strFiles += "<br />";
			}
		} while ( -1 != _findnext64(findRet, &fileinfo));
		
		_findclose(findRet);
	}

	// ���ļ��ַ���д�뵽 Content ��.
	if(strFiles.size() > 0)
	{
		puts(strFiles.c_str());
	}

	// 4. ���������־.
	puts("</body></html>");

	/*
	* ������Ӧͷ��Ϣ
	*/
	__int64 ltime;
	_time64(&ltime);

	map_content_type_string(_contentType, CONTENT_TYPE_HTML);
	format_range(_contentRange, 0, bp->size() - 1, bp->size());
	build_etag_string(_etag, bp->buffer(), (size_t)bp->size());
	_contentLength = bp->size();
	_lastModify = format_http_date(&ltime);
	_acceptRanges = false;
	return true;
}

size_t HTTPContent::puts(const char* str)
{
	assert(_content);
	return _content->write(str, strlen(str));
}

void HTTPContent::close()
{
	if(_content)
	{
		delete _content;
		_content = NULL;
	}
}

std::string& HTTPContent::contentType()
{
	return _contentType;
}

std::string& HTTPContent::contentRange()
{
	return _contentRange;
}

__int64 HTTPContent::contentLength()
{
	return _contentLength;
}

std::string& HTTPContent::lastModified()
{
	return _lastModify;
}

std::string& HTTPContent::etag()
{
	return _etag;
}

bool HTTPContent::acceptRanges()
{
	return _acceptRanges;
}


__int64 HTTPContent::_size()
{
	if(_content) return _content->size();
	return 0;
}

size_t HTTPContent::_pump(reader_t reader, size_t maxlen)
{
	return 0;
}

size_t HTTPContent::_read(void* buf, size_t len)
{
	if(_content)
	{
		return _content->read(buf, len);
	}
	return 0;
}