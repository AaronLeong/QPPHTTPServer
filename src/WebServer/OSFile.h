/* Copyright (C) 2011 ������
 *
 * ����һ����Դ������,���������ɵ��޸ĺͷ���.
 * ��ֹ������ҵ��;.
 *
 * ��ϵԭ����: querw@sina.com 
*/

#pragma once

/*
* ����ϵͳ����ʹ��C��׼�⺯�� fopen() ͬʱ�򿪵��ļ���������(һ����512, ���� _setmaxstdio() ����2048)
* ���� HTTP ��������˵��ԶԶ������,���� class OSFile ֱ�ӵ��� Windows File API ��ʵ�ֺ� C��׼�⺯��ͬ���Ĺ���.
* OSFile ͬʱ�򿪵��ļ������Դﵽ 16384,���ҿ���ͨ�� Windows ���������� --max-open-files=N ���޸�.
*
*/

class OSFile
{
public:
	/*
	* ��ģʽ
	*/
	static const int r = 0x01;
	static const int w = 0x02;
	static const int rw = 0x04;

	/*
	* seek ģʽ
	*/
	static const int s_cur = FILE_CURRENT;
	static const int s_set = FILE_BEGIN;
	static const int s_end = FILE_END;

	static bool exist(const char *fileName);
	static bool remove(const char *fileName);
#if defined(WIN32)
private:
	HANDLE _h;
#endif
public:
	OSFile();
	virtual ~OSFile();
	bool open(const TCHAR *fileName, unsigned int mode, bool tmp = false);
	bool close();
	bool trunc();
	bool isopen();
	bool eof();

	unsigned long read(void *buf, unsigned long len);
	unsigned long write(const void *buf, unsigned long len);
	__int64 seek(__int64 off, DWORD mode);
	__int64 tell();
	__int64 size();
};