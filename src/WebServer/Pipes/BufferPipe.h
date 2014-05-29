#pragma once
#include "pipe.h"
#include "..\Buffer.h"

class BufferPipe : public IPipe
{
private:
	// ˽�е�,�����಻Ҫ����Щ����
	// ���˱Ƚ�ϲ������(����)�ӿ�ʵ�ֶ���������,�����������̫ǿ��,������ȫ�˽����.
	// ����Ǵ���ӿ�,�����ⲻ��,�ӿ�������������;���������ӿ�,����ʵ���ֺܸ���,û����ȫ���,������޸�һ������ı�����?
	Buffer _buffer;
	size_t _rdPos;
	size_t _wrPos;
	size_t _maxSize;
	size_t _stepSize;

	void trim();
protected:
	// IPipe
	size_t _read(void* buf, size_t len);
	size_t _pump(reader_t reader, size_t maxlen);
	__int64 _size();

public:
	BufferPipe(size_t maxBufSize, size_t stepSize);
	BufferPipe(void* buf, size_t dataLen, size_t bufLen);
	BufferPipe(const void* buf, size_t dataLen, size_t bufLen);
	~BufferPipe();
	
	void trunc();

	/*
	*  �ṩֱ�ӷ����ڲ��������ķ���,�����ڴ濽��.
	*/
	// ���������� for write, �൱�� _write, ֱ�ӷ����ڲ���������
	size_t lock(void **buf, size_t len);
	size_t unlock(size_t len);

	// ���ֱ�ӷ����ڲ��������Ķ�ָ��
	const void* buffer();
	// size()
	size_t skip(size_t len);
};
