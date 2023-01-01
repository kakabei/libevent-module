#ifndef JMWEBSVR_PROC_H_
#define JMWEBSVR_PROC_H_

#include <event2/http.h>
#include <event2/http_struct.h>

class CHttpSvrProc  
{
public:

	static void SignalTermHandler(int fd, short event, void *arg);
	static void HttpGetVer(struct evhttp_request *req, void *arg);

	int Init(const char *pConfFile);
	int Run();

	int RetMsg(struct evhttp_request *req, const char *RetBuff);
	int GetVer(struct evhttp_request *req);

private: 
	unsigned int  m_nHost;
	unsigned int  m_nPort;
	unsigned int m_nTimeout;
};

#endif
