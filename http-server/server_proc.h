/** 
 * @file:     server_proc.h
 * @Author:   fangsh
 * @email:    1447675994@qq.com
 * @data:      2023/01/03 
 * @brief:   
 */

#ifndef HTTP_SERVER__PROC_H_
#define HTTP_SERVER__PROC_H_

#include <event2/http.h>
#include <event2/http_struct.h>
#include <string>

class CHttpSvrProc  
{
public:
	CHttpSvrProc(); 
	~CHttpSvrProc(); 

public:

	int Init(const char *pConfFile);
	
    int Run();
    
    void  SetCallBack(struct evhttp *xphttpd);

	char * FindHttpHeader(struct evhttp_request * req, 
								struct evkeyvalq * sign_params, 
								const char * key); 

	int RetMsg(struct evhttp_request *req, const char * buff);

public:
	
	static void SignalTermHandler(int fd, short event, void *arg);
	
	static void GetVerCallBack(struct evhttp_request *req, void *arg);

public:
	int GetVer(struct evhttp_request *req);

private: 
	unsigned int  m_nHost;
	unsigned int  m_nPort;
	unsigned int m_nTimeout;
};

#endif
