#include "StdAfx.h"
#include <stdarg.h>
#include "Pipe.h"
#include "BufferPipe.h"

size_t _build_pipe_chain(IPipe* p, ...)
{
	size_t count = 1;
	va_list argptr;
	va_start(argptr, p);

	IPipe* curPipe = p;
	IPipe* nextPipe = NULL;

	while(nextPipe = va_arg(argptr, IPipe*))
	{
		nextPipe->link(curPipe);
		curPipe = nextPipe;
		++count;
	}
	va_end(argptr);
	return count;
}

IPipe::IPipe()
	: _prev(NULL), _next(NULL)
{
}

IPipe::~IPipe()
{
	unlink();
}

void IPipe::link(IPipe* prevPipe)
{
	unlink();

	if(prevPipe)
	{
		this->_next = prevPipe->_next;
		this->_prev = prevPipe;

		if(prevPipe->_next)
		{
			prevPipe->_next->_prev = this;
		}
		prevPipe->_next = this;
	}
}

void IPipe::unlink()
{
	if(_next)
	{
		_next->_prev = _prev;
	}

	if(_prev)
	{
		_prev->_next = _next;
	}

	_prev = NULL;
	_next = NULL;
}

IPipe* IPipe::prev()
{
	return _prev;
}

IPipe* IPipe::next()
{
	return _next;
}

__int64 IPipe::size()
{
	return _size();
}

// �����ݶ��뵽��ǰ�ܵ���
size_t IPipe::pump(size_t maxlen /* = 0 */, IPipe* src /* = NULL */)
{
	if(maxlen == 0) maxlen = SIZE_MAX;
	if(src == NULL) src = prev();
	if(src == NULL) return 0;

	size_t plen = 0;
	while(src && plen < maxlen)
	{
		if(src->_size() > 0 || !src->_skip())
		{
			size_t rd = _pump(std::bind(&IPipe::_read, src, std::placeholders::_1, std::placeholders::_2), maxlen - plen);
			plen += rd;

			if(rd == 0 && src->_size() > 0)
			{
				// ��ǰ�ܵ����ڲ�����������,����Ҫ��������
				break;
			}
		}
		src = src->prev();
	}
	return plen;
}

bool IPipe::_skip()
{
	return true;
}

// �����ݾ����ܵ���ǰ��
size_t IPipe::push(size_t maxlen /* = 0 */, IPipe* dest /* = NULL */)
{
	if(maxlen == 0) maxlen = SIZE_MAX;
	if(dest == NULL) dest = next();
	if(dest == NULL) return 0;

	size_t total = 0;
	size_t plen = 0;

	// �ҵ���ǰ���Ŀ��ܵ�
	while(dest)
	{
		if(dest->_size() == 0 && dest->_skip() && dest->next())
		{
			dest = dest->next();
		}
		else
		{
			break;
		}
	}

	// �𼶳�ȡ
	while(dest && dest != this && total < maxlen)
	{
		total += dest->_pump(std::bind(&IPipe::_read, this, std::placeholders::_1, std::placeholders::_2), maxlen - total);
		dest = dest->prev();
	}

	return total;
}

size_t IPipe::read(void* buf, size_t len)
{
	BufferPipe bp(buf, 0, len);
	return bp.pump(len, this);
}

size_t IPipe::write(const void* buf, size_t len)
{
	BufferPipe bp(buf, len, len);
	return pump(0, &bp);
}