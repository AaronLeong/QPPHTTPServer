V1.5
====

1. ������Ҫͬ���Ĳ���,������ HttpFileServer �ĳ�Ա, ��������. 
��Ҫ�� �ؼ��ζ��� �����̺߳���.

2. ��Ҫ����״̬��Ϣ��, ����  HttpFileServer �� OnError() ����, ����һ���ṹ��.
���� N1, N2 �� STR , ֻ��ֵ, �������ַ���.
�ַ����ֽ��溯������.
ʹ�� SendMessage() ��������.


/////////////////
�ֱ���һ�� HTTPRequest �� HTTPResponse ��, ��װ��Ӧ�Ĳ���.
LinstenProc �� ServiceProc ֻ�����׽��ֲ���, ���շ���. ��������.

1. HTTPRequest.PushData() �������տͻ�������.
2. HTTPResponse.PopData() ������Ӧ���ݸ��ͻ���.
3. HTTPServerImp������дһ������ GetResponse(),����HTTPRequest����һ����ӦHTTPResponse
��ʹ�ͻ��˵Ĳ���,��ʽ����,����ʹ���˷�GET����,ҲӦ����Ӧ,��ʾ�ͻ���.


// 
V1.52Beta Build 152710

1. ��״̬����ʾ��ǰʹ�õĴ���.
2. ��״̬����ʾ��ǰһ���ж��ٸ�����.

3. _wfindfrist Ӧ�ø�Ϊ _wfindfirst64
4. ��������־ÿ��ֻ�Ƴ�ǰ��100��(ԭ���Ƴ�200��)
5. tagClientInf ���������ֶ�,�ֱ��¼���ӿ�ʼ�����ӽ����� GetTickcount ��ͳ��һ������һ��ռ���˶೤ʱ��.
6. HTTPServerImp ��� OnRecv() �� OnSend() ����,��ԭ��SericeProc�й��ڽ��պͷ��͵Ĵ���ֱ�����������������,�����߼�������.
ServiceProc �̺߳���ֻ�������� HTTPServerImp.

7. �����������������

V1.52 Build 152711
OnTimer() û����ȷͬ��. ����: �������ʱ�п��ܻ���΢С�����.

V1.52 Beta 152712
CLIENTINF ����ͳ�Ʒ����ֽ���Ӧ���� __int64 ����.

V1.52 Beta 152713
���д���ͳ�ƴ���,��д��һ������.

V1.52 Beta 152714
1. ���Ŀ¼ʱ,����1G���ļ���ʾΪ GB ������ MB.
2. ���� WSARecv �� WSASend ��ʱ,�Ѽ���������.
3. ͳ�ƴ���ʱ,�ڲ�ʹ��ԭ����ͬ��,�����Ч��.(ԭ��ʹ���ٽ�α���)
4. ���Ӹձ�����ʱ,ֻ��ȥһ��������Ϊ0�� WSARecv ����,�Խ�ʡ��Դ.(���������������û�б�Ҫ.)
5. ��һ��IOCP�̼߳��AcceptEx()������,�������Խ�ʡһ�������߳�,���ҿ�����Ч����������.

V1.52 Beta 153715
1. ��������ť����ɫ.

2. ��������ÿ��IP�����ٸ�����.
������һ��map: key = ipaddress string, value = connection count, OnAccept()�м�����map.

3. ��������ÿ�����ӵ�������.

(1) ��OnSend()��,���㵱ǰ���ӵĴ���,�������ָ��ֵ,��ô����һ����ʱ��,������MS����� OnSend()����,����ǰִ�е�OnSend()ֱ�ӷ���.
������Ҫ��һ��TimerQueue
����ȴ���ʱ�䳬�������ӳ�ʱ,��Ҫ�޸������ӳ�ʱ�������Ӷ����50MSʹ�´ε��� OnSend()ǰ������Ϊ�ٶ�������ʱ���Ͷ����ж�Ϊ������
OnSend()����һ�� bDelay �Ĳ����Ա�ʶ�Ƿ�����ʱ����.

(2) ��һ�ַ���
��OnSend()�м��㵱ǰ����,�������ָ��ֵ,�������СӦ�÷��Ͷ��ٸ��ֽ�,�����ڷ�����󻺳������.
ʹ�����𽥽��͵�ָ��ֵ.

4. �������׽���Ҳ����ɶ˿ڴ���,�Ӵ󲢷���������.

5. ���ӽ���ʱ,����ƽ���ٶ�.ע�����Ϊ0�����.

6. ��������:��һ������ͷ����Ϊ��ν���ʱ,�ᵼ�����ӱ��ر�.

7. �����Ƿ������־������.

8. ���������ļ���־.

9. �ڽ�ֹ���Ŀ¼�������,��������Ĭ���ļ���

10. ����3����ʱ������,�ֱ��Ӧ3�ֶ�ʱ��,���������.

V1.52 Build 152716

V1.52 Build 152718
1. doAccept()�е�OnAccept()������С����.
2. ��־��Ϣ�޸�,����ip��ַ�Ͷ˿ڶ�����һ�е���ǰ��.

===========
V2.00 Beta

Bug Fix list:
1. ִ���������ʱ,��������ɹ����ص��Ǵ��͵��ֽ���Ϊ0�ж�Ϊ�ǿͻ��˹ر�����.
2. HTTPServerImp::mapServerFile()�� "strFilePath.end()--" ��Ϊ "--strFilePath.end()" ����ȥ������"\"�Եõ���ȷ��·��.
3. �������ƴ������ʱ����,���µĶ�ʱ����Դй©.���������ʱ�ͻ��˹ر������ӽ���������.
4. �Ự��ʱʱ,�ڲ�ȷ����һ���߳�����״���������,ֱ�ӹر����п���ʹ�õ��׽��־��,������һ��ͬ������. ���ڸ��ĻỰ��ʱ�ļ�����.

--------------------
2011-12-04
��ҪĿ��: ��� FastCGI ֧��,ʵ�� remote ģʽ,������ locale ģʽ��չ.
���� remote ���� locale ֻ������ͨ��������,����ĺ��Ĵ��벻Ӧ���ܴ�Ӱ��.

��������ģ���ԭ��: ����ʱ��,ÿ���׽���ֻ��һ��������һ��д����һ����һ��д����.

0. ���� - CGI �� FastCGI

0.1 ʲô��CGI? ����һ����׼,���(http://tools.ietf.org/html/rfc3875),վ��Web�������ĽǶ�,������ҵķ�����֧��CGI?
0.1.1 �� UNIX ������, ͨ�� pipe(), fork(), execve(), dup(), dup2() �Ⱥ�����Web Server���̴��� CGI �ӽ���,���Ұ� stdin �� stdout �ض��򵽹ܵ�,
ͬʱ��HTTP��������һ�� env ��,ͨ�� execve() ���ݵ� CGI �ӽ��̴Ӷ�ʵ�� Web Server �� CGI Process ֮������ݴ���.
0.1.2 ��Windows ������, ʹ���ļ�ϵͳ: Web Server �Ѳ���д��һ����ʱ�ļ�Ȼ������ CGI �ӽ���, CGI �ӽ��̰Ѵ��������Ϊָ������һ���ļ�.

0.2 ʲô�� FastCGI? ����һ������Э��,���(http://www.fastcgi.com/).
0.2.1 FastCGI Application �б���ģʽ��Զ��ģʽ��������ģʽ. ��� FastCGI Application ������Զ��,��ôFastCGI Application������
һ��TCP�˿�, Web Server���Ӹö˿ں��� FastCGI Э����֮��������.
0.2.2 ����� Unix ����,����ʹ�� Unix���׽���(�����ڹܵ�)����,�����Windows����ֻ����TCP������.
0.2.3 ���ӽ�����,���� FastCGI Э�齻������,���� FastCGI Application ����������,�������ӿ��Ա���,������ÿ�ζ��ر�.

����,���Բο� Lighttpd ��Դ����.

1. Ҫ֧��POST����,�ͻ��˵ݽ���������һ��HTTPContent����, FastCGI ģ��Ӹö����ȡ����.

1.1 HTTPContent ����ӿͻ��˶�ȡ����,����chunked����? ���ټ�¼ͳ������,Ȼ���ٵݽ��� FastCGI ����.
1.2 

2. FastCGI ����Ҳһ������ɶ˿��߳�����, �� CFastCGIConn ��װ, ������tagClientInf��.

2.1 CFastCGIConn ����
2.1.1 Connect(),Close()��.
2.1.2 Proc(),��Ϊ�����¼���ɺ�Ļص�����,�ڸú����ڲ���� FastCGI Э��涨�Ķ���.

3. �Ƿ�ά��һ�� CFastCGIConn ���ӳ�?

4. �Ƿ�ά��һ�� FastCGI Application �Ľ��̳�?

2011-12-05

1. ���� CFastCGIFactory ����ά������FastCGI���̳غ����ӳ�.
1.1 CFastCGIFactory::GetConnection() ����һ����ԾFCGI����.
1.2 CFastCGIFactory::Catch() ������չ����PHP�ж��Ƿ��� FastCGI����ǰ����,�����������Ĭ�ϴ���.
1.3 CFastCGIFactory���������,�߼������еĺ�������ȫ���� CFastCGIConn ����,���Բ�������Response����,��CFastCGIConnֱ�Ӳ����ͻ��������׽���.

2011-12-06

1. ��������ģ��, IOCPNetwork, ���𴴽��׽���,�ر��׽���,����ʱ�����а��������ӳ�ʱ�ͻỰ��ʱ.
�ϲ�ģ��ͨ�� intptr_t (����SOCKET���) ����,���ǲ�ֱ�Ӳ����׽���.
����һ������ӿ�,�������ܻص��¼�,�� OnRead() OnRecv() OnClose() OnTimeout() ��.
����ֻ��һ���ص�����resultfunc_t,��Ϊ��������GetQueuedCompletionStatus()�ķ��ؽ��,����ģ�鲢��֪����send,recv����accept.

���� IOCPNetwork �������׽���,ֻ��ͨ�� registerSocket(bool add_or_remove) �Ǽ�/ȡ�� Ҫ��IOCP������׽���.
�����ƺ�������һЩ.

2. HTTPServerImp
2.1 HTTPServerImp::CatchFastCGIRequest() ����FastCGI����
2.2 HTTPServerImp::ReleaseFastCGIRequest() FastCGIģ�鴦����FastCGI�����,���øú����Ի�����Դ.

3. ���Ĵ���(���˽���)ȥ��MFC����
3.1 �� _beginthreadex ���� _beginthread ���� AfxBeginthread
3.2 ����������ʱ�⺯��(C/C++��׼�⺯�� > Microsoft CRT (MS �Ա�׼�����չ) > ֱ�ӵ���Windows API) / ���ݽṹ��STL,��Ҫ��MFC�Ŀ�.
3.3 �����ڸ������ڱ�дANSI���ݵ�UTF-8����ĳ��������UNICODE��,��Ϊ�������õ�����,ֻ��windowsƽ̨������Щ w_**** ����.
��������Ϊ ANSI���ݵĳ�����ǳ���ı�����Ŀ,����������˵����ֻ�Ǳ��뷽ʽ����Ӱ������߼�,��ȫ��ʹ��UNICODE�汾��API����С�������ʱ����������ȫû������. UNICODE�����Ǳ�Ҫ�Ķ����Ǻõ�,��UNICODE��������ANSI��������д���벻����û��Ҫ�Ķ������޴���.Ҫ���������������
��LINUXһ��,������UTF-8,�����ӿ���ANSI,��ʾ������UTF-8���ǺϺ��߼���,�����Ľ������.

2011-12-07

1. ����󶨵�������ĳ��IP��ַ��������,������־���߽����ĳ��λ����ʾ��ǰ�󶨵�IP��ַ.

2. ��HTTPServerImp�а���һ�� ServerEnv ��Ϊ asp.net / jsp / php ���ʱ�õ��� "server"�����ʵ��.
ֻ�ṩһЩ��̬��,��Ҫ���⹫��������.
2.1 mapPath()
2.2 maxConnections()
2.3 maxSpeed() / maxConnSpeed()
2.4 sessionTimeout()
2.5 deadTimeout()
2.6 rootPath();
2.7 isDirectoryVisible();
2.8 port();
2.9 ipAddress();
2.10 defaultFileNames();
2.11 curConnectionsCount(); // �� SOCKINFMAP.size() �ظ���.

3. ���԰� ServerEnv ���ݸ���ͨ����(�� HTTPServerImp �������) ���� �κ�����֧�ֵ�������FastCGI����.

4. HTTPApplication ��ʾ���HTTPӦ��,����HTTPServerImp����Ϊ HTTPServerImp ��������ʾHTTPӦ���е�һ������,���������������Ͻ������վһ��.
HTTPServerImpӵ���Լ�������ģ��,�����Լ�����������. HTTPApplication ֻ��ά��һ������HTTPServerImpʵ��.
�ƺ�Ҳûʲô��Ҫ...HTTPApplication�Ĺ���̫����,û�д��ڵı�Ҫ,�������ֱ�Ӵ������ HTTPServerImpʵ������.

2011-12-08

1. HTTPServerImp ���һ�� ServerEnv ����,����ά�� "������" ��صľ�̬��Ϣ������,�� PHP �� SERVER ��ʵ��,��Ҫ�� ServerEnv ���󴫵ݸ� 
HttpConnection �� FastCGIConnection.

2. �߼�����Ҫ HttpConnection ������ͨ��HTTP���Ӻ� FastCGIConnection ����FastCGI ����,�������� HttpConnection ���ܲ�����,����ֱ�ӰѴ���
�ϲ��� HTTPServerImp ��.

3. FastCGIConnection::proc(ServerEnv* svr ...) HTTPServerImp����proc()�ķ���ֵ�ж� FastCGI ģ���Ƿ��Ѿ���������������Ի�����Դ.

==================
2011-12-21

1. �����ȶ���(��С��)��һ���߳�ʵ�ֶ�ʱ��. ���ǲ�Ҫʹ�� Timer Queue, Timer Queueʹ����̫���߳�.

����һ��WaitableTimer��һ��Event,��¼����ĳ�ʱʱ��. ��ʱ���߳�ʹ�� Muti-Wait�ȴ���2������.
�����µĶ�ʱ����Ҫ����ʱ,SetEvent�Ƕ�ʱ�߳�����,Ȼ��Ƚϵõ���Ҫ�ȴ���ʱ��,�ٵ��� SetWaitbleTimer.


2. ������������صĹ���,���ͽ��ճ�ʱ,�ٶ����Ƶȶ����е�����ģ����.����һ���������߳�����ִ����ʱ����,��ʱ�����ں�ֻ������ʱ����,Ȼ��ͷ��ؾ������ٶ�ʱ���̵߳�ִ��ʱ��.

3. ������ģ������в���������ʵ������Ŀ��
3.1 ����ͬʱ��һ�����������һ����������.
3.2 ����ʱ����.
3.3 ���е������Ʒ����ٶ����ƺͽ����ٶ�����.

���еĲ��������������������,��Ϊ�ص�������������,��Ȼ���綯������˳��ִ��,���ǻص��������Բ���ִ��.

2011-12-22
1. ����ģ��ֻʵ�ֶ�IOCP���߳�ģ�͵���С��װ,�Ȳ���ͬ������Ҳ������ OVERLAPPED ���ݽṹ.
����ṹ IOCPOVERLAPPED ��һ���ֶ��Ա���ص������ĵ�ַ.
�����������,Ҫ�ﵽ�������պͷ���ǰ���µĳ�ʱ����,��Ҫͬ�����ƶ����ϲ������Ҳ��Ҫ��ͬ������,������Ϊ����ģ�黹������С���.

2. �ö��к�WaitableTimerʵ�ֶ�ʱ����. ��Ϊ��ʱ,�ٶ����ƵĶ�ʱ��.
�Ự��ʱ������ OnSend �� OnRecv�м��,���Ч��.

2012-1-2
1. �� QueryPerformanceCounter ʵ���˾���1ms�Ķ�ʱ������. ��Ȼ�Ա�����ûʲô����.

2012-1-18
1. ���ƻ�ȥ��... IOCPNetwork �����ص��ṹ,�ṩ��ʱ,�ٶ����ƹ���,���������������Ӧ�õ�FCGIģ����?
2. ��ʱIHTTPServer�ӿ�,������ģ��(��FCGI)�ص�,����HTTPServerImp���ܻ����Ϣ,���ѷ��͵��ֽ���,�Ƿ�رյ�.


2012-2-5
1. HTTPServerImp����ֻ���������׽���,�����ӱ����ܺ�, HTTPServerImp�������url��չ�������ʹ�����ͬ���͵�HTTP���Ӷ���.
HTTPConnection ���� FCGIConnection.
2. HTTPConnection�������һ����ͨ��HTTP����,Ŀ¼�б���߾�̬�ļ�.ֱ�Ӳ�������ģ��,����ص�����.
3. FCGIConnection�������FCGI����.ֱ�Ӳ�������ģ��,����ص�����.
4. HTTPConnection �����Ӷ�����������֮��,�ص� HTTPServerImp ����� onClose �ӿ�,����������Դ.

2012-2-9
1. HTTPContent ������ HTTPRequest �� HTTPResponse ��,���ⲿ���ɼ�.
2. HTTPRequest �� HTTPResponse �ֱ��ṩ push �� pop ����,һ��ֻд,һ��ֻ��.
3. HTTPResponse �ṩ setFile �� setData ���������ļ���������,���� HTTPContent �Ĵ���.

2012-2-13
1. HTTPRequest �ṩ recvFromClient ����,���ҹ���socket buffer, HTTPServerImp�����ڽ��յ��¿ͻ����Ӻ�,ֻ����һ�� HTTPRequest ����,
Ȼ����� recvFromClient(). ����ģ����Ȼ�� HTTPServerImp ����.
2. HTTPResponse �ṩ sendToClient ����,ͬ��.
3. FCGIResponse �ṩ recvFromFCGI, sendToFCGI, sendToClient ����.
4. ������ HTTPConnection ����Ĵ���.

5. HTTPRequest �� IResponse �ֱ��ṩ����ͳ�Ƶĺ���,������պͷ��͵�����.


========================================
1. һ��connection��Ϊ�����߼�����
1.1 HTTPRequest,������պͽ���HTTP����,�����Ӧ�������¼�.
1.2 HTTPResponder,��������Ӧ,�����Ӧ�������¼�.

2. IHTTPServer �ṩ onRequest �� onResponser �ص�.

3. FCGIResponder �� HTTPResponder �������Խӿ� IResponder

4. HTTPResponseHeader

2012-2-14
1. ֧�� keep-alive ѡ��.���ڱ����������ڴ���������.

================================

2012-2-17
ֻʣ��FCGIResponder��Ҫ������.

1. ��һ�� memfile ��ΪFCGI���ͻ���,���԰Ѷ���䳤��FCGIRecordд��memfile,Ȼ��������memfile ֮��,������һ������
�����ͽ����ʹ�ù̶������������������.

2. ������ FCGI_BEGIN_REQUEST�� FCGI_PARAMS֮��,�ͽ�������ӵ�״̬
2.1 ��HTTP��ȡSTDIN
2.2 ����STDIN��FCGI
2.3 ��FCGI��ȡSTDOUT
2.4 ����STDOUT��HTTP
�����Ĳ�����ͬʱ����,ͬ������Ӧ����ô����?

2012-2-22
1. �������ͳɹ����Ϊ�����ߴ��� FCGIResponder
1.1 HTTP -> STDIN
1.2 STDOUT -> HTTP
�������߿��Բ�������,ֻ��Ҫ��������������.
ģ�� FCGIContent , ��� eof() ֻ���յ� FCGI_END_REQUEST ֮�� eof() �ŷ���true.

2012-2-23
FCGIWriterPipe - ���ƹ������ķ�ʽ,����ͨ��������FCGIPipe �õ�����ú���������.
FCGIReaderPipe - �Ѵ�FCGIServer�յ�����������,�õ���������ͨ������
��Ҫ���Լ��Ļ�����.

2012-2-28
phpinfo()�����Ѿ��ܳɹ����еõ����.
������Ҫ�ٿ��ǳ���ṹ��������.
Ŀ��:�����ٵĻ�����ʵ��.

ʵ��һ�����ƹܵ��� socket_buffer_t, ��FCGI server �յ����ݺ����д�뻺��,�ֶ��Ҳû����,��С�����Զ�����.

2012-3-2
2.0�汾���Ҫ������:
1. ȥ�� UNICODE, ����ʹ�� ANSI-UTF8��̱ȽϺ�. ���ǳ���ı�Դ. �ַ������������ʾ����.
2. �� HTTPConf �ౣ������XML��ʽ.


2012-3-6
FCGIResponder �չ�. 
���ڿ��� FCGIFactory ����ģ�������
1. Զ��FCGI server ���ӵ�ͳһ����,����.
2. ����FCGI ���̵Ĺ���,����.
3. ������ͻָ�,Զ��TCP���Ӷ˿�,���ؽ�������Ӧʱ,Ӧ����ô��.
4. ͬһ��TCP/PIPE,���FCGI���󲢷�ʱ,��ô��.

2012-3-9
���ڶ�� FCGIResponder ��Ҫ����ͬһ��FCGI����(�׽��ֻ��߹ܵ�),���Ա����� FCGIFactory ������FCGI����.
�����Բ�ͬ�� FCGIResponder �ķ��������Ŷ�,��ȷ���������� FCGI Record ��������.
ͬʱҲ��Ҫ�� �������� �Ŷ�: FCGIFactory ���������ƵĻ������� FCGI���ӵ�����(�����ڴ�����),
������ FCGIResponder �ύ��������֮���ʹ�� FCGI������ȡ����,������Ҫ��֤ FCGI record ��������.

2012-3-11
������ FCGIResponder ����ͬһ��FCGI����,��ô���� FCGIResponder ���໥Ӱ��,�ٶȿ�Ŀͻ�����Ҫ�ȴ��ٶ����Ŀͻ���.
��Ϊ���ܻ����������.
��һ��˼·:  FCGIFactory ʵ��һ�����ӳ�,��ÿ�� FCGIResponder ����һ��������FCGI����,��������.����,��Ȼ�������Ķ�һЩ,����
�ṹ�ϱȽϼ�,�����໥֮�䲻��Ӱ��.
ȱ��: �����������ܵ�����,�����Ǳ���ģʽ��, �޷����������FCGI server����,���ĳ���ͻ��˵���Ӧʱ�䳤�ͻᵼ�¸�FCGI����(����)
һֱ��ռ��.
�ڱ�����Զ��ģʽ����FCGIserver��һ�����з���,���� nginx ����.

2012-3-12
����ʹ�����ӳصķ���,��������һ���ȴ����в���¼���ʱ��(����getConnection() ���ṩһ���ص�����). �������ӱ��ͷ�ʱ(�� releaseConnection
������ʱ)�鿴�ȴ�����,����� FCGIResponder ���ڵȴ�����ûص�����,ͬʱҪ���ȴ�ʱ��,����ȴ�ʱ�����,���� HTTP 503.
����ȴ����й���,���Ƴ���ͷ��¼������ HTTP503.

Զ��ģʽ��,�׽��ֵ� connect ������ FCGIFactory ��ִ�л��� FCGIResponder ��ִ����?
�߼��Ͽ���Ӧ���� FCGIFactory, ������ connect ��һ���첽����,����� FCGIFactory ִ��,�����Ҫ�ص�����.

2012-3-13
connect ������ FCGIFactory ִ��, �����׽��ֺ�,����æµ����,����FCGI����״̬Ϊ CONNECTING,ͬʱ FCGIResponder ����ȴ�����.
������ɺ�,�ص�.
ԭ��: ���� getConnection �õ���FCGI���Ӷ��ǿ�ֱ���õ�����.

2012-3-14
��FCGI Windows NT�������ܵ���ʹ���������.
����һ�� PHP-CGI ����ͬʱֻ�ܴ���һ������(��Ϊֻ��һ�������ܵ�ʵ����STDIN),���������ܵ�, HTTP server ������֪����������һ�������ܵ�ʵ��(PHP-CGI)�ڴ���ǰ����,����ʹ�ò�ͬ�Ĺܵ�����.
����Ҫʵ��һ�����̳�,������ά�� PHP-CGI ����. FCGIProcessContainer
�ж��ٸ� PHP-CGI ���̾��ж��ٸ������ܵ�ʵ��,���ܴ������ٸ�FCGI���Ӷ�������.

2012-3-15
ÿ��PHP-CGI���̶�Ӧһ���������Ƶ������ܵ�,��Ӧһ��FCGI����.
�������ĺô���HTTP����������ͨ��FCGI���ӵ�״̬�ж϶�Ӧ���Ǹ�PHP-CGI�����Ƿ����ڴ�������.
Ŀ����: FCGIFactory Ҫ�е��ԵĹ������ɸ�(����25��)PHP-CGI����,��ѹ���͵�ʱ��,�����������ٵ� PHP-CGI����,һ������ ���е�PHP-CGI������(�����е�FCGI������)����һ���ض���ֵ(����5��),��ɱ��һ������(����3��,ֻ����2�����н����ڿ��ж����ڵȴ���һ��ʹ��)���ҿ���ʱ�䳬��һ����ֵ(����5��)��PHP-CGI����.

2012-3-18
FCGIResponder
���������ΪFCGI server��ԭ��������ж�,��Ӧ�÷���һ�� FCGI_ABORT�����жϷ���.
�����黹��FCGIFactory�����ӾͿ�����������ʹ��.

2012-3-19
����ʹ�ûص�������ʵ��һ�ֺܲ��õ����,�Ժ����ĵ���˳��,·������Ҫ�ǳ����.
����������ͬ�����������,�����������.
�����Ƕ�Ӧ���������¼������ĳ����ûص���������ʲô�����취��? ������-������ģ��?
Խ��Խ��������ܹ�����Ҫ��,��ϧ�������̫��.

2012-3-23
Ϊ�˾�������FCGI�ű�������ʱ��,��Ҫ��������FCGI����Ӧ
1. �� memfile ���ڴ滺��.
2. �� tmpfile �ڴ����л���.

ͬ��Ҳ��Ҫ��������HTTP������
1. �� memfile ����.
2. post ���ļ���Ҫд����̵���ʱ�ļ�.

HTTP Server Ҫ����һ���ļ������ڴ����ʱ�ļ�.

HTTPRequest �Ƿ����յ���������֮��Żص� IHTTPserver onrequest?
���ȶ�,����� memcontent,������� file_content.

2012-3-24
ֻ��FCGIResponder��Ҫ���� request input, ����ֻ�� FCGIResponder �л�������.

2012-3-26
HTTPRequest �ڽ�����POST DATA֮��Żص�HTTPserver.

2012-4-1
"�˳���"
FCGI->Cache �� Cache->HTTP �����������������һֱ����,ֱ���д���������������ִ����ϲŽ���.
ÿ����������IO������ɵ�һ�̼�����һ����������״̬,���������һ���������Ѿ���������,���˳�.��"һ�̼�"�����˳���.

2012-4-6
1. FCGIResponder �˳�����:
1.1 ������һ���������ǰ
1.2 �������ʧ��ʱ

2. �Ự��ʱ���ʵ��
HTTPServerImpΪÿ�����ӷ���һ����ʱ��,��ʱ����ʱʱ,���� IRequest::stop() �� IResponder::stop(),���������Ȼ��ͬ��.

3. IOCPNetwork ����Ҫ������һ��ͬ������, onIoFinished �к�ʱɾ����ʱ���ȽϺ�?

2012-4-9
Ӧ���� mapServerFile() �д���Ĭ���ļ���������. ���� HTTPRequest, HTTPResponder, FCGIResponder �ȶ�����˵Ĭ���ļ�����͸����.

����ϵͳ��Ӧʹ�� fopen() ͬʱ�򿪵��ļ���������(2048),���֧�ָ���Ĳ�����������? �Ŷ�?
���� lighttpd ���ʵ��.

http://stackoverflow.com/questions/870173/is-there-a-limit-on-number-of-open-files-in-windows

If you use the standard C/C++ POSIX libraries with Windows, the answer is "yes", there is a limit.

However, interestingly, the limit is imposed by the kind of C/C++ libraries that you are using.

I came across with the following JIRA thread (http://bugs.mysql.com/bug.php?id=24509) from MySQL. They were dealing with the same problem about the number of open files.

However, Paul DuBois explained that the problem could effectively be eliminated in Windows by using ...

    Win32 API calls (CreateFile(), WriteFile(), and so forth) and the default maximum number of open files has been increased to 16384. The maximum can be increased further by using the --max-open-files=N option at server startup.

Naturally, you could have a theoretically large number of open files by using a technique similar to database connections-pooling, but that would have a severe effect on performance.

Indeed, opening a large number of files could be bad design. However, some situations call require it. For example, if you are building a database server that will be used by thousands of users or applications, the server will necessarily have to open a large number of files (or suffer a performance hit by using file-descriptor pooling techniques).

My 2 cents.

Luis

ֱ��ʹ�� Windows API ����/��/��д/�ر� �ļ� class OSFile ������ʹ�� C�����ļ� fopen ��. Ȼ����ʹ�õȴ����н�һ�����󲢷���.

2012-4-10
FCGIResponder ��2��ʹ���˴�ֲ������ĺ��� sendPostData() ֱ��ʹ�÷��ͻ��� reserve(), initFCGIEnv() Ҳ���,����ջ���.

2012-4-11
XmlDocument ������ʹ�õݹ麯���ĵط�ʹ��ջ��ѭ������.

2012-4-15
1. XmlDocument ���� xmlЭ��ڵ�ķ�ʽ��Ҫ������һ��.
2. XMLNode::GetNode ������Խ�������.

2012-4-16
��μ򵥵�֧�� XPath
1. ���ھ���·��ʹ�� /root/child1/child2
2. �������·��ʹ�� ./child1/child2 ���� nodename/child1/child2

Ŀǰ������������.

����һ�� class XPath 
1. ���캯������һ���ַ���.
2. bool GetFirst()
3. bool GetNext()
4. bool IsAbsolutePath()

���� 

XPath(const std::string &path, XmlDocument *doc);
XMLHANDLE XPath::GetNode();

����: XPath ��ֱ�Ӳ��� XmlDocument �û���ֻ���ṩ���� XPath �ַ����Ĺ��ܺ�?

2012-4-17
XMLNode ���뻹��UNICODE,�ò��� UTF-8?
OS_Conv() �Ľӿ��������
int OS_Conv("utf-8", "gb2312", src, srcLen, dest, destLen);
Windows ƽ̨�µľ���ʵ��Ҫ�������е����ı���.

====================
�Ľ�XMLģ��,�����ڴ��ʹ����
1. �������� LoadNode() ֱ�ӽ��� char�ַ�������.
2. �������� LoadNode() �ɷֿ����,ÿ��״̬�����Իָ�.
3. ������� GetNode() �ɷֿ����.

2012-4-22
v0.2��Ĺ����Ѿ�ʵ��.
1. IOCPNetwork �� XmlDocument �����໹��Ҫ�ٸĽ�.
2. XPath Ҫʵ��,���� xml ģ�����������������. ���ؽڵ㼯��ʱ��,��������������ָ��ļ���,����һ�� new �� listָ��, ��һ�����װ��,�������Զ�ɾ��.

===========================
2012-5-17
1.�������� FCGIResponder �������������Ĵ���,���ڵĴ���д��̫������.
2.���ȫ����ģʽ.

====
2013-8-22

V0.3

1. �ùܵ��ĸ����д: HTTP����ͨ�� Socket/SSL �ܵ��ύ�� IRequest, IResponder ͨ��: File/Buffer�ܵ� -> Socket/SSL�ܵ�
IResonderͨ��: File/FCGI�ܵ�->Cache�ܵ�->Socket/SSL�ܵ������ݰ������ͻ�ȥ.

2. ��д�׽��ֵĹ������,ÿ��socket����һ��������,Ӧ�ò��IOCP�ֱ���һ������Ĳ���,Ȼ��ģ���"�ɶ�""��д"�� select ״̬.
�������԰�SSL,Linuxƽ̨�Լ�Windowsƽ̨�ĸ�������ģ��ͳһ����,ȱ������Ҫ��һ���Ļ�����.

3. �ص����������Ķ��Ķ���Ӧ������.

==
2013-8-26

1. Pipe ������ BufferedPipe
2. BufferPipe + FilePipe ���þ��� FIFOCache

== 
2013-8-27

1. Pipe ˫������,���ǳ�,д����.
2. FCGIResponderFactory / DefaultResponderFactory �ĸ���
3. �� Pipe ��read write��Ϊ�ڲ�����,��_prev����ʱ�ر��ⲿ write ����,��_next����ʱ�ر��ⲿ read ����.��������Ŀ����ȷ����������������.����������.

==
2013-8-28

1. "��ִ�е�Ԫ" �ĸ��� IExecutableUnit.
   Q++ HTTP Server ������ IHTTPServer �������ɸ� "��ִ�е�Ԫ" ÿ�� unit �����Էֶ�ռ������ʱ��,���ǲ���ϵͳ�еĽ���һ��������.
   ��ִ�е�Ԫ���� ����, HTTP����,HTTP��Ӧ��,ÿ�� unit ��������ض��Ĺ��ܻ�������.

2. ExecutableUnitScheduler(������) ���ȸ�����Ԫ,�Ѵ��� �ɶ�/д ״̬�� unit ѡ��,������ IExecutableUnit::exec()
   ������Ҫ�� unit ע�ᵽ �������� token = socket / �ļ� �ľ��.

==
2013-8-29

1. IOCPNetwork ��ʱ������ PostQueuedCompletionStatus ����һ���ض��İ�.
��֮Ӧ�þ����� PostQueuedCompletionStatus ʹ����ִ�������ӹ����߳�������ڳ���.

2. ExecUnit ���� socket/handle ��Ϊ token

==
2013-8-30

1. IOCPNetwork ����һ��,�� add() ������Ϊ�׽���/���������������(�����ӳٵ�ʵ�ʶ�дʱ����������).
2. �ⲿ������дʱ,ֻ��д IOCPNetwork ����Ļ�����,�ͺ�����ͨ���첽�׽�������,ֱ�ӷ���. IOCPNetwork �ڲ���ͣ�ĸ��� IOCP ģ�͵ķ��ؽ������ڲ�������.
3. HTTPServerImp �䵱������,������̵߳��� IOCPNetwork::select() Ȼ��ͨ�� iocp_key_t �ҵ���Ӧ�� IExecUnit,�ٵ��� exec()�����������.�ڲ�ʹ�ùܵ�ϵͳ.�����V0.3������ṹ.
4. IExecUnit���� iocp_key_t,������ػ���ʱ����.

2013-9-1

1. IOCPNetwork ���������߳�,�������д�����ˢ�¶�����. �ڲ�����������,�ֱ𱣴�ɶ�/��д���׽���.
2. �����̺߳�select֮�����߳�ͬ��������ͨ��.
3. ���ע���˼����ɶ��¼�,��ôIOCPnetworkӦ�÷���һ�� WSARecv ���� AcceptEx ���� (getsockopt SO_ACCEPTCONN).
4. ConnectEx ������ɺ�,������д�¼�. SO_CONNECT_TIME �����ж��׽����Ƿ�����.
3.4 Ӧ���� add �д���,����һ��Ӧ���� IOCPNetwork ����.

XXX Connect �� accept �ĵ�һ����Ӧ�õ���.
����Ӧ����˵���� Connect һ��,���Ϸ��� IO_PENDING,Ȼ�������д�¼�,���ж��Ƿ��������.
���� accept һ�η��� IO_PENDING,Ȼ������ɶ��¼�,���ж��Ƿ��������Ƿ����.
����߼����ǱȽ���Ȼ��,���Խ���.
��Ȼ add ��ȻҪ�ж� 1. listening ���׽��� 2. un connect �׽��� 3. connected �׽���
ֻ�� 3 ��ֱ�ӽ����д����.

add() ע��.
modify()�޸ļ����¼�.

===
2013-9-10

V0.3 ����ܽ�

Ŀ�깦��: ֧��SSL,֧��GZIP, deflate ѹ����

1. ��۷���, HTTPServerImp �������ɸ���ִ�е�Ԫ(IExecUnit),�������ʵ���ʱ���л�ִ��֮,�������������������ʽ.
(1) IExecUnit ���� HTTP����,HTTP��Ӧ,SSL����,�����׽���,�ȷ��ǿ��Էֲ�ִ�е�״̬��.
(2) �����˻ص�������ʽ, HTTPserver �������ĵ��ø�ִ�е�Ԫ,��������ǵ�״̬.

2. ΢��ϸ��
(1) �ܵ�ϵͳ,����,��ǰд��������.������Ӹ��ֹ�����ʵ������GZIPѹ��֮��Ĺ���.
(2) Responder factory��˼��. HTTPserver����ʵ���� Default responder factory ��������Ĭ��IHTTPResponder.

3. ����ģ��.
Ϊ��Ͽ�ִ�е�Ԫ����ư�IOCP��װΪ����select��ģ��,����Ҳ�ʹӽṹ��Ϊʹ������������ģ��(��windows select, event select, linux poll��)������׼��.
(1) ÿ��socket����2��������.
(2) ��̨���̲߳�ͣ����д/��ջ�����,Ȼ��ʹ���¼�֪ͨ.

==
2013-9-13

1. ����ص�,�����ṩ IHTTPServerStatusHandler �ӿ�,�����ṩһ��״̬��ѯ�ӿ�,��UI����������ѯ. �Ƿ����,�Ƿ�ʱ?
2. �ع�������,�����˶���޸�,��ô����? �Ⱥ����΢��,�ȰѴ�ṹ������?

==
2013-9-19

1. ����,�Ӵ�ṹ��ʼ.
2. ����: HTTPListener ����һ�������Ӻ�,�����ӵ� HTTPServerImp ��?
 2.1 �ص��ķ�ʽ onNewConnection()
 2.2 addConnection()
 2.3 ���� exec �ķ���ֵ���ദ��,�������Ļ��ƺ�Υ���� OO ��ԭ��,��װ�� EXECUNIT ���� exec Ȼ������Ҫת��Ϊ����������Ե���ĳЩ����.
 ����"��OOģ����,���Ǿ��ñ�Ť�ĵط�,�����ģ�Ϳ϶�������"��ԭ��,��Ҫ���¹�˼.
 �����������ԭ���� HTTPServerImp �� HTTPListener ������(ǰ�߰����ں���)�����õͲ㷴�������ø���о��ͺܱ�Ť.��������߷���ͬһ��,�����ټ�һ��ͳһ�������ǻ�õ���?

 ==
 2013-9-23

 1. HTTPServerImp ���ݵĽ�ɫ
 1) �ṩIHTTPServer��ʵ�ָ���������(IRequest, IResponder ��)����
 2) ʵ�� Core Driver ��ɫ,��������ģ����������ִ�ж�������.
 3) ��Ϊ�ⲿ���õ��������.

 2. ��ȫ��OOP,����IExecUnit�����������,ͨ������ IServer �ķ���ʵ���߼�.��ͻص��ķ�ʽ�ڱ������ǲ�һ����˼��.

==
2014-3-6
1. IOCPNetwork �ĳ��첽�׽�����Ϊģʽ,�ڲ�������,��̨���/��ջ�����.��� select ����.
����Ҫ ioadapter�ӿ�.

2014-3-8
1. Pipeϵͳ��� pump()�ӿ�������ȡ���ݵ��ڲ�������.
2. ��shutdown��ʹiocp����֮����closesocket,�����ȽϺ�.

2014-3-10
1. IOAdapter ��װ�׽���,���ĳ�� send IOCP����ֻ���ͳɹ���һ����,��ô����? ��̨�������ͻ�������һ����д״̬?
��̨���߳�ˢ�»�����..
send(buf == NULL)��ʾ��������.

IOCPSelector ��Ŀ������epollһ��

2014-3-11
1. IOCPSelector ����һ��/����߳�ˢ������IOAdapter�Ļ�����,����һ����Ծ�������.(�¼�,ͬ������ȵȵĿ������ٲ��˵�)
һ����3�ַ���.
(1) �ⲿ���� select ��ϵͳIOCP�����л�ȡһ�������. (��Ҫ������IO����ֻ�ǲ�����ɵ����,��Ϊ���ⲿ����select��������ģ�鲻�ܼ�ʱˢ��,��sendΪ��,���跢��1KB�����ݶ�IOCP���������512KB,��ôʣ���512KBʲôʱ����?��˭���������send����? ��Ȼ�����ṩ�ӿ�(����IOAdapter���ṩ�ӿڲ�ѯ�ڲ��������Ƿ�������,�ṩһ�� send(NULL, 0) ������Լ���������ͻ�������ʣ�������)������,������������. ����Ϊ���ǲ������ŵ���ƾ�����ȱ�ݵ����,��ôȱ��������?
(2) ��(1)����,ֻ����select��������ֲ���������Զ�������һ��IO����Ȼ���ٵݹ����select ȷ��һ��IO������������ɲŴ�select�з���.(���ǲ��ܴӸ����Ͻ��,��Ȼʵ��Ӧ�����ⲿ����һ����Ѹ�ٵ���select,�������߼�����˵��������.
(3) ��һ����̨�߳�ˢ�»�����,�ⲿ����ֻ�ӻ�Ծ������ȡ���.(�߼��Ͽ���ʹ����ģ�����,�����кܶ�ͬ������,����ά������).

��ԭ���õ�"�ص�����"�����,����ʹ��IOCPģ������Ȼ��Ȼ���뷨,�����ص�����������Ƶ��³��򱻶�����,һ�������������.

2014-3-12
����(3)�����IOCPSelector (��̨�߳�ˢ��buf).
1. ���ź�����ʾ��ԾIO������еĸ���.
2. IOAdapter ͨ�� lockbuf �� unlockbuf �ӿ��ṩ�ڲ��������ķ���,���� SocketPipe �Ͳ����ٷ��仺������.

2014-3-14
1. IOCPAdapter ��� ctl ����ʵ�ֺ� epoll_ctl���ƵĹ���,�����Ϳ��Խ�� recv/send ��ʱ��ʼ��ʱ����������.
2. recv �������� SOCKET_ERROR(-1) lasterror = EAGAIN ʱ������һ�� WSARecv IO����.
3. send ���������ں�̨�Զ������Ѿ����뻺�����ڵ�����,ÿ��WSASend IO��ɶ������Ծ����/���ߵ����ͻ��������ʱ����.

2014-3-15
1. recv �߼�: ctl ������ӿɶ��¼�ʱ,�Զ�����һ�� WSARecv IO,IO������ɺ�ˢ�»�����,�ȴ����ݱ���ȡ,��ʱ��ʹ������δ��Ҳ��������һ�ζ�IO,���������ڵ����ݱ���ȡ���(�� recv ����-1ʱ),�����Ȼ��ע�ɶ��¼����Զ�������һ�ζ�IO.������ƿ��Ա�������һ�ζ�IO(���һֱ�Զ������IO,�������ԶԷ������ݶ��Ѿ��������,��ô����һ�ζ�IO�޷���ɻ��߳�ʱ).
2. send �߼�: ��̨�� WSASend дIO��ջ�����,�������߳�,ֻ������ͬ���Ϳ���,ֻҪ ctl ����˿�д�¼�,��ôÿһ��дIO��ɶ������Ծ����,���û��ע��д�¼�,��̨�߳���ջ���������ͣ.
3. accept �߼�: ���ǹ�ע�ɶ��¼�,���� listen �����ú��Զ����� AcceptEx IO,�Լ� accept�����ú�Ҳ����.

2014-3-16
1. recv/send ������ WinSock һ��,�����̲߳���ͬʱ���� recv.
2. ������һ��ԭ��,��� send ������̨�߳���ջ�����,ÿ��IOCP���ض�������д�¼�,��ô�ͻ�����������,���� send���߼���Ϊ�ύ���ݺ�,Ͷ��IO����,ֱ��IOCP����,�������һ��û�з������,�����Ͷ��IOֱ������ȫ��������ϻ��߷�������,Ȼ���ٴ�����д�¼�.�����Ͳ���Ҫ��ͬ����.

2014-3-17
1. IOCPģ��epollʽ��װ�������(��ʱ���ٶ����ƻ�û���),��ƶ���. recv/send����һ��IO��ɺ󴥷��ɶ�/��д�¼�; recvֻ�������ݶ��������ô�����Żᴥ����һ��IO;sendֻ�����ύ�������ں�̨ȫ��(���ǲ���,���Կ���ҪͶ�ݶ��IO)������ϲŴ�����д�¼�;һ���׽������ͬʱ���Խ���һ�� RECV IO ��һ�� SEND IO;ͬʱֻ����һ���̵߳��� recv/send ����(����ƵĽǶ���˵Ҳû��Ҫ����̶߳�ͬһ���׽���ͬʱ����recv/send).
2. ��һ�����TimerQueue����,�Ƿ�����¼�ʽ��ƴ���(Ŀǰ�ǻص�ʽ���)Ҳ���Կ����� IOCPSelector�������ź���/������ʵ��,��������ͳһ���,����ص�.
���� v0.2���ʹ�����,����TimerQueue��bug(�ܿ�����ĳ��ͬ��û�����)

2014-3-18
1. TimerQueue ����Ҫ��������,ֱ����ͬ�������²�����ʱ�����м���.���������Ƕ����. ������?��Ҫ���Թ�һ��.

2014-3-19
1. ȷ�� FCGIResponder �ĳ���ִ������BUG(��������,��ô��Ծ,ͬ��û�����,����FCGI��������HTTP����������ֹ��,û����������Ͳ����г�ʱ����ʧ��,���ڽ���״̬,������־����Ϊ�������һֱû�д�����,Ҳ���ᳬʱ),�������е�FCGI������Թ���������. ��ȻV0.3��Ҫ��д�������,��ʱ�Ȳ�ȥ��V0.2��BUG��.
2. ״̬�ص��ӿڲ��Ǹ������,HTTPServerImp�ṩһ��״̬��ѯ�ӿڴ���,��UI��ÿ��һ��ʱ��������ѯ��ǰ��������,�������Ϣ.

2014-3-21
1. ����ACCEPTEXʱ,�ύһ����С���ȵĽ��ջ���,������accepted���׽��־ʹ��ڿɶ�״̬,����˺�ʱ��˭�����õ�һ��recv������,ͬʱ��������Ҳ���Ա���.
3. ����ģ���Ҫ���ٽ���: һ��ֻ����һ��IO�ڲ���,�����Ͳ���Ҫ��ͬ����,ʵ���ϴﵽ���Ҫ����㹻��,��������ͬʱ���IO��Ƶı�Ҫ.
4. �ٶ����ƵĹ��ܷ��� IOCPSocket ������? �ǲ���Ӧ�÷��� HTTP���� ���������? HTTPServerImp �� IExecUnit run() ����sleep,�����������ٶ�.

2014-3-22
1. IOCPAdapterҪ�������������в���,����ʵ�ְ�ȫ�ر�,ͬʱ��������Ҳ��ʵ����.(������ͬʱ�ж�� send/recv ��)
2. ������ȫ�Ĺرղ��Ǹ�����
Ҫվ�� IOselector�ĽǶ��������������: IOAdapter����״̬�²���ɾ��,ɾ��֮��Ͳ����ٷ���.Ҫ�������Ҫô����,ͬ�����Ʊ���״̬һ����;Ҫô��Զ���̷߳���
_onIoCompleted ������IOCPSelectorҪ������,������ IOAdapterΪ����״̬(���߼�������״̬),������ close �����еõ�״̬����һ�µ���.(ԭ������������ close �õ� Adpater״̬Ϊ����,ɾ��,����_onIoCompleted�ַ����˱�ɾ����Adapter)
IOCPAdater���һ������ onLastIOComplete ר�����ڴ�����һ��IO�Ĳ������(���ü���,�ؼ�,��������˵ onLastIOComplete �����ϴ�IO������,���޸�oppType,���Բ��ü���),����һ������ clean ��������״̬,��Ҫ�������autodelete

2014-3-24
IOCPģ���ͬ������

1. IOCPAdapter��ͬ��,ά���ڲ�״̬��ͳһ, oppType �ֶ�����. close() - recv/send/connect/accept��������Ҫ����sock����ĺ��� - onIOCompleted() 3���߳���Ҫͬ��

2. IOCPSelector��ͬ��
2.1 ��Ծ������Ҫͬ��,����ð�,���ź�����lock���Խ��.
2.2 close() ʱ,ɾ�� IOCPAdapterָ��� _onIOCompleted ˢ��IOCPAdapter����֮����Ҫͬ��.

����ͬ��,2��������Ҫ����,�Ƿ���Ҫ,��ô��.
Ҫ����Ƿ���Ҫ��ͬ��������,����ȷһ����������ԭ��:
(1) ����һ�� IOAdapter,����ͬʱ�����һ�� recv��IO��һ�� send��IO(����ͬʱͶ��һ��recv IO �� һ�� send IO,���ǲ���ͬʱͶ������ recv IO),���Ҫ����Խ��͵��κ�ʱ�����ֻ��һ�� IO
(2) �ⲿ����ͨ�� epollʽ�ĳ���ṹ�õ���Ծ��IOAdapter.
(3) �г�ʱ����
(4) ������IO����û����ɵ�ʱ��ǿ�ƹر�IOAdapter(�����ⲿ��������˵,close()֮���������, IOCPSelector�ں�̨Ҫ����Ҫ����β����.)

2014-3-25
1. ������ص��ù���(������һ���̵߳��� recv ���� send ֱ������ -1,Ȼ�����´οɶ�/��д�¼�֪ͨ���ټ�������) IOCPAdapter ����Ҫ��ͬ��.  (recv �� send �Ľӿ�Ҫ�������)
2. IOCPSelector ��һ��ȫ����ͬ�����в���.

�����Ҫ֧��ͬʱRECV �� SEND ��ôӦ����������,���ֻ֧��ֻ��һ��RECV ���� SEND IO,��ô���Բ����� IO_DOUBLE = false

olp�ṹ���errcode�ֶ�����������ԵĴ�����,getlasterror��������ֵ����һ������ֵ,�����ͻ�������ͬ���Ϳ���ʵ�� recv / send ͬʱ����. ��ʱ�ر�ʱ��һ���������ܻ������߳�ͬʱ����һ���׽��־��һ���ر�,��һ�����ͻ��߽���,��������?���Ǽ�����.

2014-3-28
1. IOAdapterʵ�� IPipe�ӿ��Է����ڲ�������.

2014-3-29
IOCP������η�װ��Ҫ����.����IOCPģ�鲻���г�ʱ����(��ʱ������IOExecUnitģ�����,exec���س�ʱʱ��,HTTPServerImp������ʱ������,���˳�ʱʱ�����exec����,���ٶ���������)
��ôIOCP��recv io �Ϳ���ʼ�ս���, send ioҲһֱ�ں�̨����һ���߳�����,ֻ�л����������ջ���д��֮��,�Ŵ����ɶ�/��д�¼�.
��ʱ�������������Ӧ��������ģ��֮��? ����΢���û�и�IOCPģ����ӳ�ʱ����?��������ʱ,�����ǶԷ�û�з���,Ҳ������������,��Ϊ����ģ��,�޷��ж��������㳬ʱ,ֻ����һ���֪��.���ͳ�ʱ�������һ��.
��֮,Ҫô����ģ����ȫ���ܳ�ʱ,Ҫô����ģ���ֱܷ����io���Զ�����,�򲻳�ʱ,���÷���,�����ó�ʱ.

2014-3-31
1. IOCPģ�������ȫ�߳�ͬ��,��̨�̲߳�ͣ��ˢ��/��ջ�����. IOCPAdpater�ڲ���ͬ��. IOCPSelector���û��ζ��в�����.
   IOCPģ�鲻�ٹܳ�ʱ���ٶ�����
2. V0.3��Ҫ���������ڹܵ�ϵͳ��IOSelector,ֻҪ������ģ�����,�����ͼ���.
3. ����ʹ���˻���,�ڵ���send�ɹ������Ϲر��׽��ֵ��������Ҫ����: ���ǰ��ձ�׼�� shutdown ���÷�
3.1 ���ݷ�����Ϻ�,���� shutdown �رշ��Ͷ�,ע�� EPOLL_EVENT_ERROR �¼�,���ȴ�.
3.2 ��̨��IOCP�ķ�ʽ�Զ���շ��ͻ����ڵ�����.
3.3 ����ȫ��������Ϻ�,����һ�� EVENT_ERROR �¼�,�����û��Ϳ���֪��ʲôʱ����Ե��ùر���.

2014-4-1
1. IOAdapter�Ĺر���Ȼ�Ǹ�����,�������������ָ�Ч��?(��Ҫ��,��Ҫ��,û���ڴ�й¶)) ����Ҫ�������.����������ʹ���������. �����������Ƴ�����ȷ��˳��ִ��,�Ϳ���ȥ��.
1.1 ��close �� onIoCompleted ִ��˳��.
1.2 ÿ�������̶߳���һ�����п�����?
1.3 IOCPAdapter��ͬ���ǲ��ɱ����.

2014.4.2
1. ʹ��2��IOCP���,һ���������ʵ�ʵ��������,��һ������֪ͨ��һ��ĵȴ�����(�����Ͳ���Ҫһ�����õĶ���,��߲�����,��ȻIOCP����Ĳ�����Ҳ����).
2. ��ȻIOCPAdapter�������ɱ���,�Ǿͱ�¶�����ṩ IOCPAdapter::lock() �� IOCPAdapter::unlock() �ø��Ե�����ͬ��ɾ������,�����ǲ����ܽ��������?
3. �����̰߳�ȫIOCPSelectorֻ�ܱ�֤��close ���غ�,��������IOAdapter��epoll�����з���.
4. ������Ӧ���ڽ��յ��ɶ�/��д�¼�֮��,�ȵ��� ctl ����,�ٵ��� recv/send ����,�����Ҫ��ȷ�����¼�֪ͨ�Ļ�,���˳�����Ҫ.

2014.4.3
 * ������:
 * 1. IOCPAdapter �� send/recv ������Ҫռ������(�ֲ���)�ͺ�̨�����߳�ͬ��.
 * 2. ��̨�����߳���ĳ��IO������ɺ���Ҫռ�þֲ���������㲥��
 * 3. �ӹ㲥�˻��IO�¼�֪ͨ��Ҫռ�þֲ���.
 * 4. ɾ��ʱ��Ҫռ��ȫ����.
 * 
 * �ֲ���ʹ��̫Ƶ����Ӱ������(���Ƕ�ȫ�ֵĲ�����Ӱ�첢����),ɾ��ʱռ��ȫ����Ӧ��û��,�Ƚ�ɾ������������.
 * ��һ���Ż�Ŀ������������پֲ�����ʹ��.

 1. ���onIoCompleted �в����� doRecv/doSend �Ϳ��Ա�֤һ��һ��IO,����Ҫͬ����,����Ч�ʸ���?

 2014.4.5
 1. BufferPipe ��pump����������Ʋ��ܸ���Ч�������ڴ�?

 2014.4.7
 1. ctl �Ĺ��ܷ��� IOCPAdapter ��,�߼��ж�Ҳ������. ����ᵼ���ؽ��������,
 2. IOCPSelector wait ��������ǰ���Զ���� ctlע����¼�(�� epoll_wait һ��),�¼�������Ϻ�Ӧ���ٴε��� ctl MOD ���¼�����ע��һ��(Ҫ����ͬ��,��ʲôʱ��Ӧ��֪ͨ,ʲôʱ��Ӧ��֪ͨ event trigger)
 ������ctl �������ж�,�������ctl����ʱ�Ѿ��������ڻ�������,�򴥷��¼�(�����ͺ� LT ģʽһ����)
 ����IOExecUnit�ķ���ֵ�ж��Ƿ���Ҫ�ٴε��� IOSelector �� ctl ����, IOSelectorҪ�ֶ�����һ���¼��ͷ�����.
 3. �ȸ���� Linux��epoll�ı��(�����Ƕ��̱߳��)ϸ����˵.
 3.1 LTģʽ: ֻҪ���� epoll_wait ��ʱ��,ĳ���׽�����������/���߻����пռ�Ϳ��Է���(���������ǲ������յ���)
 3.2 ETģʽ: ����epoll_waitʱ,û���µ����ݵ���/�����������һ����,��ô�Ͳ�֪ͨ. ����:������µ����ݵ���,���ǻ�����û��,֪ͨ��֪ͨ��?
 3.3 ����̵߳��� epoll_waitʱ,����Ѿ�������һ���׽���,�����¼�������,���ǻ����������? ���Ǳ���Ҫ�� IO_EVENT_EPOLLONESHOT ������?

 ԭ������ETģʽ�����Ӧ��ʹ��ȷ��,ֻ�л�������/�յ�״̬�仯ʱ�Ŵ���,ֻ��Ӧ�ý�һ���� ctl �������ж��ǲ���Ӧ�ô���, ���� ctl ������read ������������,�ж�ʧ�Ŀ���.
 ����: �ȵ��� read ֪������ -1 no = EAGAIN,��ʾ���ں�̨��������,Ȼ���ٵ��� ctl ע��ɶ�,���������̨�� read �� ctl �����м�ʱ������˶�,��ʱ�׽��ֲ�û��ע���¼�(ctl��ûִ��),Ȼ��ctlִ��ע���˿ɶ��¼�,�Ǿ�������.

 4. ��LTģʽ+ONSHOTӦ�ÿ��Խ������.Ч��Ҳ��������.
 ��������ģʽ,���� ctl ����ʱ����׽����Ѿ����ڿɶ�/��д״̬��Ӧ�ô����¼�(�ֶ�����).
 ���ڶ��̵߳��� epoll_wait,����Ҫ��� ONSHOT ,���ctl ���������������,��ôETģʽ��LTģʽû������.

 2014.4.9
1. ��� HTTPConnection��.
2. exec �ķ���ֵ��� struct execres_t

2014.4.10
1. IO_ERROR_PENDING �ǲ����� IO_ERROR_SUCESS ����
2. shutdown ����Ҫ�ٿ���ϸ��.
3. ʵ�ֹܵ������ӵĴ���.

2014.4.12
1. HTTPRequest ��ΪIOExecUnitִ����Ϻ���ô��HTTPServerImpͨ��?
Ŀǰ���� onRequest �ص��ķ�ʽͨ��,�ܾ��ò����.�����ҵ�ԭ��,���Ǿ��ñ�Ť�ĵط�,�϶�����Щ����û�п������,�����ģ��������.
�뷨1: ��COMһ������ QueryInterface �����ķ�����IOExecUnit�еĵ���Ӧ�Ľӿ�. Ҫ�� exec �����÷���ֵ����. �ƺ�Ҳ���Ǻܺ�,ֻ��һ��ȡ�ɵķ�������.

2014.4.13
�������õ�״̬��,���һ��Ư���Ļ�����б�Ҫ.

2014.4.14
�����Ǹ�����,ѡ���Ծ�� IOAdapter ֮����ô����
1. ���� IOAdapter map �õ� IOStateMachine Ȼ������֮. ����������޸�ɾ��,������ɾ���Ǹ�����,��������(�ڳ�Ա������ɾ���Լ�?)���Һ����׳���.

2014.4.15
HTTPServerImpΪÿ��ע���IOStateMachine ����һ���ṹ,������һЩ��Ϣ�����������ʱ���õĺ���ָ��,����IOStateMachine�Ͳ�����Ҫ���Ļص�����.

2014.4.16
����״̬����״̬��������. �������������ʵ���ʱ������IO״̬������,���Ҹ������н������Ԥ��ע��Ĵ�����.
����,������������Ҳ����ͨ���������ӿڵ��������Լ���״̬��,����FCGIFactory.

2014.4.18
����ģ�黹�ǲ���(����ģ���Ǻ���Ҳ������,�����,��������Ľṹ,Ч�ʶ���ȥ,�㲻��,������������ô�������±�����).
Ŀ��: adpater �� selector �������,˵���׾����� Windows �� IOCP ����ģ�� Linux �� epoll.

1. adapter ֻ��һ�������ӿ�,ά�� SOCKET ���,����������в���Ҫ�������Ĳ���,��setsockopt,bind,listen ��.
2. selector ά�� iocp�ṹ,�������� olp �� ���ԵĻ������Ѿ�һЩ���������ñ���.
3. adapter û�м��뵽 selector ʱ,ֻ����ɲ���Ҫ�������Ĳ���(���������Ӧ����adapter û�м��뵽selectorǰʵ�������׽��ֻ����첽�׽��ֵĹ���,���� socket û���� select / epoll ǰһ�����������Щ����һ��,���ڷ�������˵ûʲô����,�Ժ�������).
4. selector��װ��������ģ�͵�ϸ��,���Ը�������ģ���и��Ե�selectorʵ��,����iocpselector,epollselector,sselector,eventselector��,����ֻ���� iocpselector.
5. adapter ���뵽 selector ֮��,iocpselectorΪÿ��adapter����һ�� iocp_context ������������, adapter Ҳ�õ����ָ��, ������Ҫ������֧�ֵĹ����û�ͨ�� adapter�Ľӿڲ��� iocp_context ʵ��.
6. iocpselector�ں�̨Ҳͬ��������� iocp_context ָ��,ˢ�»�����,�����������¼�.
7. �ر��׽���ʱ, �û�����Ӧ�õ��� ctl ������ selector��ɾ��(����������Ȼ��),��ʱ�����adapter�� iocp_context ָ�뱻�ջ�(����ɾ��),�û��Ϳ���ֱ��ɾ�� adapterʵ��,ʵ����selector �� adapter�ķ���. iocpselector���ܻ���Ҫ�ں�̨��һЩ������,��ɺ���ɾ�� iocp_context ָ��.
8. ʵ��ʱ adapter �� selector �ǳɶ�ʵ�ֵ�,�� iocpadapter �� iocpselector, ��Ϊֻ�� iocpadapter ��֪����β��� iocpselector �ṩ�� iocp_context ����.

����
1. Linux epoll ������ִ�� io ����ӵ� epoll �м��״̬. ��ôʵ��?
2. AcceptEx ��ɺ��յ��ĵ�һ��������ô����?
3. doRecv / doSend ʲôʱ�����?

0. ������ִ�� io �ټ��״̬,ֻ���ȷ��� iocp_context,Ȼ����� ctl ��ӵ� selector ʱ,��ָ��ί�и�selector����,һ��ί�оͲ������ջػ��߹ر�ʱ���û��IO�������ڽ���,���û�оͿɻ�ȡ��ί��,�ջ�ָ��.��������Windows IOCPģ�����������岻��,û�����ʵ����Ҳ��������io,�������˸�����. ctl ADD ʱ���� ctx ָ��, ctl DEL ʱ�ջ�ָ��(��ioselectorɾ�����ߵ�IO��ɺ�ɾ��) ���� ctx ָ���� selector ͳһ����,����ԭ��.
1. ���ӱ�������,�û�ʹ��adapterͨ��selector���ж��Ƿ�ɶ���д,����ν��һ�����ݵ�����.
2. ȫ�ֹ��ߺ��� iocp_recv / iocp_send / iocp_accept / iocp_connect ���� iocp_context �ṹ������IO. ��֮ adapter ����Ҫ֪�� selector �Ĵ���;selectorֻ����ͨ�� ctl ��ӽ����� adapter.
3. adapter �� selector ���п��ܵ�����Щȫ�ֹ��ߺ���(�߼�����˵,iocp_context ������� adapter֮��, adapter ��ȻҲҪ������)
4. selector ά��һ�� iocp_context �б�(����������ʹ�õ�,Ҳ�������Ѿ������Ϊɾ����),��������selectorʱ,���Խ������ adapter �� iocp_context ������,�Ӷ����� adapter �� selector.

2014.4.20
1. �ƺ��� IOCPAdapter ������ IOCPContext �ȽϺ���? ����ĳ�� selector �Ͱ� ctx ί�и��Ǹ� selector ����, �˳�ʱ�ջ�.

2014.4.22
IOCPContext �����������. Adapter �� Selector ��ȫ����Ļ�,�����߼��޷���֤ɾ�� Adapter ֮ǰһ������� ctl DEL, ���� Adapter �� Context ����һ�� socket handle �޷�ά�����ݵ�һ����,���ܵ��¶���߳�ͬʱ����.

1. ��� wakeup ����
2. selector����adapterʱȫ�������ж�����delete��.
3. clone �������������õ����׽���

2014.4.23
Accept ����˭,��ʱ���� AcceptEx �õ����׽��־���Ǹ�����. ������Ϊ������⵼�� IOCPAdapter������� Selector��ָ��,���Ǻܶ��ĵ�. �����.
�������ǵ��� IOCPContext. �û������� ctl ADD ����������� ctl DEL ���ǿ��ԺϺ��߼���; Adapter �ڷ���ʱ, ʹ IOCPContext ����� socket = INVALID_SOCKET.

2014.4.30
1. ֱ����HTTPConnection��д��־? �����Ͳ���ͳ����Ϣ��?
2. �� ����IOSelector ͳ������ģ�������(����/�������ʵ�)

2014.5.2
1. ��ʼ�޸�FCGIģ��.
1.1 FCGIRecord ��һ��Pipe,����������,FCGIRecord������.
1.2 FCGICache û��Ҫ��,ֱ���� BufferPipe + TmpFilePipe ����. 

2. FCGI ������.
2.1 Server -> Client: FCGI IOAdapter (NamedPipe) -> FCGIRecordPipe -> FilePipe -> BufferPipe -> HTTPResponderHeader -> HTTP Client IOAdapter
����ͬ��,˫�������� FCGi IOAdapter write, HTTP Client IOAdapter pump, �� FilePipe �� BufferPipe �ṩ���湦��.

2014.5.4
FCGIFactory ��һ����״̬��,�������к�FCGI������������(���ػ���Զ�̶�����ΪIOAdapter).
��FCGIResponderʱ,���õ�������reinstall�����Ӱ�װ��Responder,Responder����ʱ��reinstall��factory.

2014.5.5
1. FCGIFactory�Ǹ��������fcgi���ӵ�״̬��
2. ����ģʽ�������ǲ��ǿ���ֱ�����ӽ��̵�stdin�����д����? ���߸���һ��Ȼ�󴫵ݸ��ӽ���,����һ��Ҫ����createfile?��������
3. iocpadapter���캯��ֱ�ӽ��ܾ��,����,���캯���еĴ�������쳣��ô����? ���C++����ģ���Ǳ���. ���� operator !()

- ����
1. ok
2. ֻ�ܰ���0.2��ķ�ʽ,������̳и��ӽ���,Ȼ��wait, createfile. ֱ�����������߼��Ͻ���ͨ,��linuxpipe��һ��.
3. ���캯������ֻ��ͨ���쳣���ߵ����ṩһ����Ա�������ж�״̬,���߹��캯��ʲô������,�����ṩһ��init��������ʼ��(�����initһ��ҲҪ�ṩrelease�����,ǿ��֢).

2014.5.6
1. FCGIFactory��ǰ������Ӧ��û�д������.
2. FCGIResponder ���Э������ IOAdapter�Ǹ�����.
����һ��IOAdp �ȴ���һ��IOAdpʱ STM_PAUSE������һ��adp��ͣ,�����Ǻ�ʱ��˭����. ������ȴ��γɿ�ת,�϶��ǲ��е�.
����STM_PAUSE������һ��������

2014.5.7
STM_PAUSE ����ʵ��,������ʵ��֮,����ģ�鲻�����.
��ͬһ������������һ��״̬����������,��������һ��,״̬��֮�������,����Ƕ��ioadp��Ӧһ��״̬���Ļ�,�߼��ϱȽϺ�һЩ:pauseһ��״̬����ĳ��adp,Ȼ����ͬ״̬������һ��adp resume.

STM_ADDADP
STM_DELADP
STM_PAUSE
STM_RESUME

fcgifactory ����fcgi adp ��װ�� fcgiresponder,������ fcgiresponder���������adp��ָ��. �������� httpadp, fcgiadp ͬʱ��װͬһ�� fcgiresponder������.
��֮,һ�����Ӳ����ж��ٸ�adp,ֻ��װһ��״̬��->httpconnection stm.

2014.5.8
1. ����״̬�����������adp�ķ�ʽ�ܸо���Щȡ��.������˵Ӧ���ɵ����������ɾ��ĳ��״̬����adp,���ǵ���������״̬�����õ���.
2. STM_ADDADP �Ĳ��� stmadpdesc_t ������adp,epollev,��ǰepollev.
3. �������е�״̬��ctx�ɴ಻Ҫ����adp����Ϣ,û������. ͨ�� adp ��userdata ���Եõ�������Ϣ,���������û������.

2014.5.11
1. FCGIFactory �ĵȴ�������ô����? ���������,�������е�FCGI���Ӷ�æµ,��ʱ��ô����?

2014.5.12
1. FCGIFactory catcheRequest ��,���û�п��е�����,��� responder����ȴ�����,Ȼ���½�һ������,����׼��������,��װ���ȴ������е�responder.
responder���ȵȴ���д�¼�ȷ�����ӿ���. responder��ondone,onabort������ FCGiFactory����.

2014.5.16
1. FCGIFactory dispatch ��˼·������,�������󶼽��ȴ�����,Ȼ����ͳһ����. ���������FCGI����ʧ����ô����?
1.1 �Զ����½�����,����ܵ���ʧ��ѭ��(��������ԭ���޷��������ӵĻ�����ѭ�����ٴζ�����ɹ�)
1.2 ������Զ���������,�����ȴ����е�FCGIResponder�Ὡ��.
1.2.1 FCGIResponder ���ó�ʱ.
1.2.2 FCGIFactory ��������ʧ�ܵĻ�,�����һ������ʧ��ʱ,��յȴ�����,ʹ���еȴ��е�FCGIResponderȫ���Ի�ȡ����ʧ�ܷ���.

2014.5.18
1. FCGIRecordReader / Writer ֱ��д Buffer ���� Pipe?
2. FCGIRecordFilter ���� Pipe? 
2.1���͵�FCGI������: FCGIRecordFilter -> BufferPipe -> FCGI Adapter
2.2��FCGI����������: FCGI Adapter -> FCGIRecordFilter -> FCGIResponder
2.3���͵�HTTP�ͻ��� FCGIResponder -> FilePipe -> BufferPipe -> ChunkFilter -> HTTPResponderHeader -> HTTP Adapter

2.2 �� 2.3 ����ֿ�,����ȫ������һ��. ��Ϊ��FCGI���������͹��������ݰ�����־,������Ϣ��,��һ����Ҫ���͵�HTTP�ͻ���.
��ȻӲҪ���������Щ��Ϣ�����йܵ�����һ��Ҳ�ǿ��Ե�,����...��Ť,�Ͳ�Ҫ������.
2.2 �� 2.3 ����ͨ��֮���� FCGIResponder��Э��.

FCGIFilter ��Ҫ��������һ�� FCGIRecord ����ʵ��. FCGIFilter�����ԭ�� FCGIRecordWriter �� FCGIRecordReader �Ĺ���.

2014.5.19
1. ���� FCGIRecordFilter / FCGIRecordPipe �����
1.1 �� FCGIRecordPipe(������) ��pump���ڲ�������,�����ΪFCGIRecord��; д��ʱҲ����д�뵽�ڲ�������. ���������в��ֹ��ܺ� BufferPipe �غ���,������.
1.2 Pipe ���� peek �� putback �� FCGIRecordFilter (�޻���) ʵ��,����������,ȴ������. (δ�������޻���Filter�������,��ChunkFilter)

2. ����������
2.1���͵�FCGI������: FCGIRecordFilter -> BufferPipe -> FCGI Adapter
2.2��FCGI����������: FCGI Adapter -> BufferPipe -> FCGIRecordFilter -> FCGIResponder
2.3���͵�HTTP�ͻ��� FCGIResponder -> FilePipe -> BufferPipe -> ChunkFilter -> HTTPResponderHeader -> HTTP Adapter

2014.5.22
1. ipipe �� read / write �߼�Ӧ��ʹ��ѭ������ _read / _write ������0ΪֹȻ���ڵ��� prev() / next() �� read() / write
2. Uri �ඨ��� URI_TEXT ���� _T
3. FcgiInputFilterר����������post(�����ط�ֱ���� FCGIRecord ����� BufferPipe). ������ BufferPipe (�����ڲ�����Ҳ����)
�߼�: �ڲ�bp���������ȡ����;������� prev size д��header ���ڲ�bp, �� prev ��ȡ���ݵ� bp,�����ص�һ��.�ڲ�����һ�������ж��Ƿ��Ѿ�д���������־�Է���0��������.
�Ƿ� Filter ���� ���� buffer Ϊ׼,��Ҫ����������,������ΪҪ.

4. Ҫʵ����������Ĺܵ�ϵͳ, peek, size �޷�ʵ��,Ҳ������. �������й���������,�����Ǽ�ʱ���ɵ�,�޷������ܵĳ���,Ҳ����"peek"�ٻָ�.
skip ����ʵ��,��Ϊ read ������, ��ȡ���ٶ���. ����������Ӧ�ô����е�ûһ��ipipeʵ������,���������еĹ�������Ч.

2014.5.23
1. pipe transformer �� bp ����,������ֻ����һ�α任�Ľ��.
read �߼�: �ڲ���������ȡ; prev ��ȡԤ������󳤶�, �任, ������, goto begin
write �߼�: �����ȡ,д��next, �任Ԥ������󳤶�, ������, д�뻺��, goto begin

2. �ŷ��ֹܵ�ϵͳ�� write �߼���ȻŪ����.
�ܵ������������ǵ������ ���Ǵ� prev -> current -> next.
������ next �˵��� pump ��ȡ����, Ҳ������ prev �� ���� push ��������. ���ַ�����������������һ�µ�.

2014.5.25
1. ���� _read / _write ���麯��,�����ȽϺ����. _pump / _push ��������,�������.
2. �� _size ��Ӧ, ���һ�� _space ��������ȷ���ڲ�������������д��������. ��Ȼ�޷� _putback ��Ҫȷ������һ���ܵ����������ݿ�����ȫд��Ŀ��ܵ�,�������ݾͻᶪʧ.

3. ˼·: ����������, Ҫ��һ�� pipe �ж�ȡʱ,�ṩ������,���Ǹ�pipe����д; Ҫдʱ,ͬ��,��Ŀ�껺����������. �����Ͳ���������ݴ�һ���ܵ��ж���,����д��Ŀ��ܵ�ʱʧ�ܵ��µ����ݶ�ʧ.
����Ŀ��ܵ�������д���Ը���Ŀ��ܵ����Ե��ص��Ż�,Ч�ʸ���.
4. ���û�ֻ�ṩ pump(IPipe* src, maxlen) �� push(dest maxlen), �����ṩֱ�� read/write buffer(ͨ��BufferPipe�����ͬ���Ĺ���).

2014.6.5
1. ���������Ƶ�ԭ��, IPipe ������ֻҪ�ṩ _read _write _size _space ���. ֻ�Ƕ༸���ڴ濽��.

2014.6.11
1. ��������� FCGIResponder ����һ��,��ôֻ��д�� V0.2�����Ķ��Ĵ���.
2. ��дһ��������״̬�� FCGIConnection ,���� ����/���� FCIG ����������. �ṩһ�� IPipe �� wait ����������ʱ����֪ͨ(��һ��Ҫ��ͬ������ʵ��,FCGIFactory ���Դ���һ���ȴ�����,ĳ�� FCIGConnection ������ʱ,ֱ�ӻ�������Ӧ�� FCGIResponder)
3. HTTPResponder �Ľ�һ�¿��Խ���һ�� FCIGConnection �ṩ���� IPipe ����Դ. ���� FCGIResponder ��û�д��ڵı�Ҫ��.
4. ���� FCGIConnection �ǳ��ڴ���,���������õĻ�����/��ʱ�ļ�Ҳ���ڴ��,Ч�������.
5. Ҫ�л��Ʊ�����ʱ�ļ�һֱ���������