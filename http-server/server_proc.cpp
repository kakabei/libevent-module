/** 
 * @file:     server_proc.cpp
 * @Author:   fangsh
 * @email:    1447675994@qq.com
 * @data:     2023年01月03日 星期二 14时27分05秒
 * @brief:   
 */

#include "server_proc.h"
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include "util/inifile.h"


#include <string>
#include <signal.h>
#include <event2/event.h>
#include <event2/listener.h>
#include <event2/keyvalq_struct.h>
#include <event2/event_struct.h>
#include <event2/buffer.h>
#include <arpa/inet.h>


struct event_base* base;

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

int CHttpSvrProc::GetVer(struct evhttp_request *req)
{
	char * uri = (char*)evhttp_uri_get_query(req->uri_elems);
	if (!uri){
		fprintf(stderr, "Error: query uri failed.\n"); 	
		RetMsg(req, "{\"ret\":-1, \"msg\":\"param err\"}");
		return -1;
	}

	char * decode_uri = strdup((char*)evhttp_uri_get_query(req->uri_elems));
	struct evkeyvalq http_query;
	evhttp_parse_query_str(decode_uri, &http_query);
	free(decode_uri);

	const char * product	= evhttp_find_header(&http_query, "product");
    const char * guid		= evhttp_find_header(&http_query, "guid");
	const char * zone		= evhttp_find_header(&http_query, "zone");
	const char * ver		= evhttp_find_header(&http_query, "ver");

	char new_ver[64] = "10.20.30";

	fprintf(stdout, "Get ver product=%s,guid=%s, zone=%s, ver=%s, new_ver=%s\n",product, guid, zone, ver, new_ver); 

	char buff[1024] = {0};
	snprintf(buff, sizeof(buff), "{\"ret\":0, \"msg\":\"ok\", \"product\":\"%s\", \"new_ver\":\"%s\"}", product, new_ver);

	RetMsg(req, buff);

	evhttp_clear_headers(&http_query); // 释放参数链表
	return 0;
}

void CHttpSvrProc::HttpGetVer(struct evhttp_request *req, void *arg)
{
	CHttpSvrProc *pProc = (CHttpSvrProc*)arg;
	if (pProc){
		pProc->GetVer(req);
	}else{
		fprintf(stderr, "Error: get arg failed.\n"); 
	}
}


int CHttpSvrProc::Run()
{
	base = event_base_new();
	if (!base){
		fprintf(stderr, "Error: careate event base failed.\n"); 
		return -1;
	}

	// 注册信号处理
	struct event evsignal;
	event_assign(&evsignal, base, SIGTERM, EV_SIGNAL|EV_PERSIST, SignalTermHandler, &evsignal);
	event_add(&evsignal, NULL);

	struct evhttp *httpd = evhttp_new(base);
	if (!httpd){
		fprintf(stderr, "Error: create evthttp_new failed."); 		
		return -1;
	}

	evhttp_set_timeout(httpd, m_nTimeout);
	evhttp_set_cb(httpd, "/get_ver", HttpGetVer, this);

	struct evhttp_bound_socket *pEvHttpHandle = NULL;
	struct evconnlistener * pEvListener       = NULL;

	struct sockaddr_in stSockAddr;
	memset(&stSockAddr, 0x0, sizeof(stSockAddr));
	stSockAddr.sin_family = AF_INET;
	stSockAddr.sin_addr.s_addr = m_nHost;
	stSockAddr.sin_port = htons(m_nPort);

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

	pEvHttpHandle = evhttp_bind_listener(httpd, pEvListener);
	if (pEvHttpHandle == NULL){
		fprintf(stderr, "Error: evhttp_bind_listener failed.\n");	
		return -1;
	}

	event_base_dispatch(base);
	evhttp_free(httpd);
	event_base_free(base);

	return 0;
}
