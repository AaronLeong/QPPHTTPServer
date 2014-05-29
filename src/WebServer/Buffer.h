/* Copyright (C) 2012 ������
 *
 * ����һ����Դ������,���������ɵ��޸ĺͷ���.
 * ��ֹ������ҵ��;.
 *
 * ��ϵԭ����: querw@sina.com 
*/

#pragma once

#ifndef _FIFOBUFFER_HEADER_PROTECT_
#define _FIFOBUFFER_HEADER_PROTECT_

/*
 * ������������
 * ����һ���ڴ�(�ⲿ�̶����ȵĻ����ڲ�������������),�ṩ���Ʊ�׼�ļ�IO�Ľӿ�.
*/

class Buffer
{
private:
	bool _innerBuf;		// �������Ƿ����ڲ�����(���������ʱҪ�ͷ�)
	char *_buffer;		// ��������ַ
	size_t _bufLen;		// ����������
	size_t _pos;		// �α�λ��
	size_t _len;		// ��Ч����
	size_t _maxSize;	// ��󳤶�
	size_t _memInc;		// �ڴ���������
	bool _readOnly;		// ֻ��

	size_t space();
	size_t reserve(size_t s);

	/* 
	* ��ֹ����,�ڴ�ָ�����õ����ⲻ�ô���,Ŀǰ�ṩ��ʵ����
	* �����·����ڴ�Ȼ����. �ܾ��ò��Ǻܺ�.
	*/
	Buffer(const Buffer &rh);
	Buffer& operator = (const Buffer &rh);
	Buffer& assign(const Buffer& rh);
public:
	/*
	* �������캯���ֱ������Է����ڴ��ָ���ڴ�
	*
	*/
	Buffer(size_t memInc = 1024, size_t maxSize = SIZE_T_MAX);
	Buffer(void* buf, size_t len);
	Buffer(const void* buf, size_t len);
	~Buffer();

	size_t read(void *buf, size_t len);
	size_t write(const void *buf, size_t len);
	int seek(long offset, int origin);
	size_t size() const;
	size_t capacity() const;
	bool eof() const;
	void trunc(bool freeBuf = true);
	size_t tell() const;
	const void* buffer() const;

	/*
	* �ӵ�ǰ�α��λ�ÿ�ʼ�����ڲ�������. �ڲ���������������,����ֱ��ʹ��ָ����"д"����,ͬʱ��Ҫ���ʱ�ķ���,���������.
	* ����������len��ʾ�ⲿ������ֱ����ָ��"д"�� len �������������α� _pos Ҳ��Ӧ������ len.
	*/
	size_t lock(void **buf, size_t len);
	size_t unlock(size_t len);
};

#endif