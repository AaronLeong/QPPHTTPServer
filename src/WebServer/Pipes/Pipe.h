#pragma once

/* Copyright (C) 2013 ������
 *
 * ����һ����Դ������,���������ɵ��޸ĺͷ���.
 * ��ֹ������ҵ��;.
 *
 * ��ϵԭ����: querw@sina.com 
*/

/*
 * "�ܵ�"��V0.3�ĳ������(2).
 * һ��HTTP����������Ϊһϵ�е���������:HTTP����ͨ�� socket �ܵ�"��"�� Server
 * Server������ܵ����ӵ�һ�� file �ܵ�/Fcgi �ܵ���,Ȼ��ʹ֮�������������һ��HTTP
 * ����.
 * �ڹܵ�ϵͳ��,ֻ���ڹܵ�ͷ"��",ͨ�������������ݴ�������һ��ĸ����ܵ���"��"����.
 * ֻ���ڹܵ�β"д",ͨ��д����������"��"��ȥ.
 *
 * ��������: (data) -> |prev| -> |this| -> |next| -> (data)
 * ���þ�̬���� buildChain �����ܵ���
 * �ܵ���һ������,ֻ��ͷβ���˶�д.
 * �������ǵ����.
 *
*/

/*
 *  һ��FCGI��Ӧ��������: FCGI IOPipeSource -> ResponderHeader -> TmpFilePipe -> BufferPipe -> HTTP IOPipeSink (������FCGI���)
 *  ���� FCGI IOPipeSource -> TmpFilePipe -> BufferPipe -> ResponderHeader -> HTTP IOPipeSink (����fcgi���)
*/

/*
* pump / push ���߼�������ȫһ��(�� a.pump(b)  != b.push(a), ���� pump �� push ���ⲿ���ֳ���ʱһ����,���ǹܵ�ϵͳ�ڲ���һ��.
* pump ������ݳ鵽Ŀ��ܵ�,����Ŀ��ܵ�,���ݲ�����洢�������ܵ�.
* push ����������͵���ǰ��,�����ǰ�˹ܵ��洢����,���ѹ���ǰ�˹ܵ�,��������.
*/

/*
* �ܵ�ϵͳ�Ļ���.
*/
#include <functional>
typedef std::function<unsigned int (void* buf, unsigned int len)> reader_t;
class IPipe
{
private:
	IPipe* _prev;
	IPipe* _next;
	
protected:
	IPipe();

	virtual size_t _read(void* buf, size_t len) = 0;
	virtual size_t _pump(reader_t reader, size_t maxlen) = 0;
	virtual __int64 _size() = 0;

	/* �Ƿ�����������ǰ�ܵ�ֱ�Ӱ�����д����һ���ܵ��� */
	virtual bool _skip();

public:
	virtual ~IPipe();

	/*
	* ���ӵ���һ���ܵ�����
	*/
	void link(IPipe* prevPipe);
	void unlink();
	IPipe* prev();
	IPipe* next();
	
	/*
	* �ܵ������������ǵ������ ���Ǵ� prev -> current -> next.
	* ������ next �˵��� pump ��ȡ����, Ҳ������ prev �� ���� push ��������. ���ַ�����������������һ�µ�.
	* Ҫ�ṩ size() ��Ӧ���ṩ pump(), ͬ�� space() �� push().
	*/
	__int64 size();
	size_t pump(size_t maxlen = 0, IPipe* src = NULL);
	size_t push(size_t maxlen = 0, IPipe* dest = NULL);
	size_t read(void* buf, size_t len);
	size_t write(const void* buf, size_t len);
};

/*
* �Ѳ���������pipe������,����Ҫ�����һ��NULL��β
* ʵ��: _build_pipe_chain(p1,p2,p3,p4,p5,NULL) ������һ��������Ϊ: p1 -> p2 -> p3 -> p4 -> p5 �Ĺܵ���
*/
size_t _build_pipe_chain(IPipe* p, ...);
#define build_pipes_chain(p0, ...) _build_pipe_chain(p0, __VA_ARGS__, NULL)