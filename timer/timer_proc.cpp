/** 
 * @file:     timer_proc.cpp
 * @Author:   fangsh
 * @email:    1447675994@qq.com
 * @data:     2023/01/04
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

#include "timer_proc.h"
#include "util/ini_file.h"

struct event_base *                 base;
static struct timeval               lasttime;
static int                          interval;

CTimerProc::CTimerProc()
{
    
}

CTimerProc::~CTimerProc()
{

}

int CTimerProc::Init(const char *pConfFile)
{
    kkb::CIniFile iniConf(pConfFile);
    if (!iniConf.IsValid()){
		fprintf(stderr, "Error: conf file is not valid, config=%s\n", pConfFile);
		return -1; 
	}

    iniConf.GetInt("TIMER", "Timeout", 2, &interval);

    // 创建基础事件 event_base_new
	base = event_base_new();
	if (!base){
		fprintf(stderr, "Error: careate event base failed.\n"); 
		return -1;
	}

	fprintf(stdout, "Timer Info:Timeout=%d\n",interval); 

	return 0;
}

int CTimerProc::Run()
{
	// 注册信号处理
	struct event evsignal;
	event_assign(&evsignal, base, SIGTERM, EV_SIGNAL|EV_PERSIST, SignalCallBack, &evsignal);
	event_add(&evsignal, NULL);

    int flags = 0;
    struct event timeout;
	struct timeval tv;

	// Initalize one event 
	event_assign(&timeout, base, -1, flags, TimeoutCallBack, (void*) &timeout);
	
    evutil_timerclear(&tv);
	tv.tv_sec = interval;
	event_add(&timeout, &tv);
    evutil_gettimeofday(&lasttime, NULL);

	// 分发事件
	event_base_dispatch(base);
	event_base_free(base);

	return 0;
}

void CTimerProc::SignalCallBack(int fd, short event, void *arg)
{
	fprintf(stdout, "Info: SignalTermHandler=%d, event=%d\n", fd, event); 
	event_base_loopbreak(base);
}

void CTimerProc::TimeoutCallBack(evutil_socket_t fd, short event, void *arg)
{
	struct event *timeout = (struct event *)arg;
	
    Handler(fd, event, timeout); // 任务处理

	struct timeval tv;
	evutil_timerclear(&tv);
	tv.tv_sec = interval;
	event_add(timeout, &tv);// 重新设置一下
}

void CTimerProc::Handler(evutil_socket_t fd, short event, void *arg)
{
    struct timeval newtime, difference;
    double elapsed;

	evutil_gettimeofday(&newtime, NULL);
	evutil_timersub(&newtime, &lasttime, &difference);
	elapsed = difference.tv_sec + (difference.tv_usec / 1.0e6);

    fprintf(stdout, "Info : TimeoutCallBack called at %d: %.3f seconds elapsed.\n",
	    (int)newtime.tv_sec, elapsed);
    
	lasttime = newtime;
}

