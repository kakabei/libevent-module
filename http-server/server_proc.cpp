/** 
 * @file:     server_proc.cpp
 * @Author:   fangsh
 * @email:    1447675994@qq.com
 * @data:     2023/01/03
 * @brief:   
 */

#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <string>
#include <signal.h>
#include <event2/event.h>
#include <event2/listener.h>
#include <event2/keyvalq_struct.h>
#include <event2/event_struct.h>
#include <event2/buffer.h>
#include <arpa/inet.h>

#include "server_proc.h"
#include "util/ini_file.h"

struct event_base* base;

CHttpSvrProc::CHttpSvrProc()
: m_nHost(0)
, m_nPort(8080) 	
, m_nTimeout(30)
{

}

CHttpSvrProc::~CHttpSvrProc()
{

}

int CHttpSvrProc::Init(const char *pConfFile)
{
    char szHostStr[32] = {0};
	int nPort 			= 80; 
	int nTimeout 		= 30; 

    kkb::CIniFile iniConf(pConfFile);
    if (!iniConf.IsValid()){
		fprintf(stderr, "Error: conf file is not valid, config=%s\n", pConfFile);
		return -1; 
	}
    
	iniConf.GetString("HTTPSVR", "Host", "0.0.0.0", szHostStr, sizeof(szHostStr));
    iniConf.GetInt("HTTPSVR", "Port", 8080, &nPort);
    iniConf.GetInt("HTTPSVR", "TimeOut", 30, &nTimeout);
       
    m_nHost 	= (unsigned int)inet_addr(szHostStr);
	m_nPort 	= (unsigned int)nPort; 
	m_nTimeout 	= (unsigned int)nTimeout; 

	fprintf(stdout, "Http Server Info: Host=%s, Port=%d,TimeOut=%d\n", szHostStr, nPort,nTimeout); 

	return 0;
}

void CHttpSvrProc::SignalTermHandler(int fd, short event, void *arg)
{
	fprintf(stdout, "Info: SignalTermHandler=%d, event=%d\n", fd, event); 
	event_base_loopbreak(base);
}

int CHttpSvrProc::RetMsg(struct evhttp_request *req, const char *buff)
{
	// 返回数据
	evhttp_add_header(req->output_headers, "Content-Type", "text/plain");
	evhttp_add_header(req->output_headers, "Connection", "close");
	evhttp_add_header(req->output_headers, "Cache-Control", "no-cache");

	struct evbuffer * evBuff = evbuffer_new();
	evbuffer_add_printf(evBuff, "%s", buff);
	evhttp_send_reply(req, HTTP_OK, "OK", evBuff);
	evbuffer_free(evBuff);

	return 0;
}

char * CHttpSvrProc::FindHttpHeader(struct evhttp_request * req, 
										struct evkeyvalq * sign_params, 
										const char * key)
{
    if (req == NULL){
        fprintf(stderr, "Error: query req failed.\n"); 
        return NULL ;
    };

	char * uri = (char*)evhttp_uri_get_query(req->uri_elems);
	if (!uri){
		fprintf(stderr, "Error: query uri failed.\n"); 	
		RetMsg(req, "{\"ret\":-1, \"msg\":\"param err\"}");
		return NULL;
	}

	char * decode_uri = strdup((char*)evhttp_uri_get_query(req->uri_elems));
	evhttp_parse_query_str(decode_uri, sign_params);
	free(decode_uri);

    char * szValue  = (char *)evhttp_find_header(sign_params, key);  

	return szValue; 
}

int CHttpSvrProc::GetVer(struct evhttp_request *req)
{
	struct evkeyvalq sign_params = {0};
	char * szProduct	= FindHttpHeader(req, &sign_params, "product");
	char * szGuid		= FindHttpHeader(req, &sign_params, "guid");
	char * szZone		= FindHttpHeader(req, &sign_params, "zone");
	char * szVer		= FindHttpHeader(req, &sign_params, "ver");

	evhttp_clear_headers(&sign_params); // 释放参数链表

	if (szProduct == NULL || szGuid == NULL || szZone == NULL || szVer == NULL){
		fprintf(stderr, "Error: query uri param err.\n"); 	
		RetMsg(req, "{\"ret\":-1, \"msg\":\"param err\"}");
		return 0;
	}

	std::string strNewVersion = "10.20.30";
	fprintf(stdout, "Get ver product=%s,guid=%s, zone=%s, ver=%s, new_ver=%s\n",
				szProduct, szGuid, szZone, szVer, strNewVersion.c_str()); 

	char szBuff[1024] = {0};
	snprintf(szBuff, sizeof(szBuff), "{\"ret\":0, \"msg\":\"ok\", \"product\":\"%s\", \"new_ver\":\"%s\"}",
				szProduct, strNewVersion.c_str());

	RetMsg(req, szBuff);

	return 0;
}

void CHttpSvrProc::GetVerCallBack(struct evhttp_request *req, void *arg)
{
	CHttpSvrProc *pProc = (CHttpSvrProc*)arg;
	if (pProc){
		pProc->GetVer(req);
	}else{
		fprintf(stderr, "Error: get arg failed.\n"); 
	}
}

void  CHttpSvrProc::SetCallBack(struct evhttp *httpd)
{
	evhttp_set_timeout(httpd, m_nTimeout);
	evhttp_set_cb(httpd, "/get_ver", GetVerCallBack, this);
    
    return;  
}

int CHttpSvrProc::Run()
{
	// 创建基础事件 event_base_new
	base = event_base_new();
	if (!base){
		fprintf(stderr, "Error: careate event base failed.\n"); 
		return -1;
	}

	// 注册信号处理
	struct event evsignal;
	event_assign(&evsignal, base, SIGTERM, EV_SIGNAL|EV_PERSIST, SignalTermHandler, &evsignal);
	event_add(&evsignal, NULL);

	// 创建 http 事件
	struct evhttp *httpd = evhttp_new(base);
	if (!httpd){
		fprintf(stderr, "Error: create evthttp_new failed."); 		
		return -1;
	}

	// 设置回调
    SetCallBack(httpd); 
	
    struct evhttp_bound_socket *pEvHttpHandle = NULL;
	struct evconnlistener * pEvListener       = NULL;

	struct sockaddr_in stSockAddr;
	memset(&stSockAddr, 0x0, sizeof(stSockAddr));
	stSockAddr.sin_family = AF_INET;
	stSockAddr.sin_addr.s_addr = m_nHost;
	stSockAddr.sin_port = htons(m_nPort);

	// 绑定端口
	pEvListener = evconnlistener_new_bind(base, 
										NULL, 
										NULL,
										LEV_OPT_CLOSE_ON_FREE|LEV_OPT_REUSEABLE|LEV_OPT_CLOSE_ON_EXEC,
										1024,
										(struct sockaddr*)&stSockAddr, 
										sizeof(stSockAddr));
	if (pEvListener == NULL){
	    fprintf(stderr, "Error: create listener failed.\n"); 	
		return -1;
	}

	// 监控端口
	pEvHttpHandle = evhttp_bind_listener(httpd, pEvListener);
	if (pEvHttpHandle == NULL){
		fprintf(stderr, "Error: evhttp_bind_listener failed.\n");	
		return -1;
	}

	// 分发事件
	event_base_dispatch(base);
	evhttp_free(httpd);
	event_base_free(base);

	return 0;
}
