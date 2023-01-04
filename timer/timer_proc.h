/** 
 * @file:     timer_proc.h
 * @Author:   fangsh
 * @email:    1447675994@qq.com
 * @data:      2023/01/04 
 * @brief:   
 */

#ifndef TIMER_PROC_H_
#define TIMER_PROC_H_

#include <event2/http.h>
#include <event2/http_struct.h>
#include <string>

class CTimerProc  
{
public:
	CTimerProc(); 
	~CTimerProc(); 

public:

	int Init(const char *pConfFile);
	
    int Run();

public:
	
	static void SignalCallBack(int fd, short event, void *arg); 
	
    static void TimeoutCallBack(evutil_socket_t fd, short event, void *arg); 

    static void Handler(evutil_socket_t fd, short event, void *arg); 
private: 


};


#endif // TIMER_PROC_H_
