#pragma once
#include "Pipes/BufferPipe.h"

/*
* �任��
* read / write ���߼��� IPipe ��һ��
* IPipe: �����ǰ�ܵ���������,����ֱ�Ӵ���һ���ܵ���ȡ���ͻ���������.(�����������˵�ǰ�ܵ�)
* IPipeFilter: ���е����ݱ��뱻��ǰ�ܵ�����.(���������������ǰ�ܵ�)
*/
class IPipeTransformer : IPipe
{
private:
	/*
	* _packageSize ��ʾһ�ο��Ա任����󳤶�.
	* �ڲ��������Ỻ��任�������,Ӧ��ȷ�� _packageSize ����������ڲ��������޷�����,���ݻᶪʧ.
	*/
	BufferPipe _bp;
	size_t _packageSize;
	byte* _packageBuf;

	size_t doTransform(const void* ibuf, size_t ilen);

protected:
	// �任
	virtual size_t _transform(const void* ibuf, size_t ilen, void* obuf, size_t olen) = 0;
	virtual size_t _bound(const void* ibuf, size_t ilen) = 0;

	size_t _read(void* buf, size_t len);
	size_t _pump(reader_t reader, size_t maxlen);
	__int64 _size();
	bool _skip();

public:
	IPipeTransformer(size_t packageSize);
	virtual ~IPipeTransformer();
};