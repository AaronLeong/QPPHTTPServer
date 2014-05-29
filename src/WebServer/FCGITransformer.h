#pragma once
#include "IPipeTransformer.h"
/*
* ���� FCGI_DATA / FCGI_STDIN / FCGI_STDOUT ���͵�����
*
*
*/
class FCGITransformer : public IPipeTransformer
{
private:
	bool _end;
	u_short _fcgiId;
	u_char _fcgiType;

	size_t _transform(const void* ibuf, size_t ilen, void* obuf, size_t olen);
	size_t _bound(const void* ibuf, size_t ilen);
public:
	FCGITransformer(u_char fcgiType, u_short fcgiId, size_t packageSize);
	virtual ~FCGITransformer();
};

