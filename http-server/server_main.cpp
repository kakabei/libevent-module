/** 
 * @file:     server_main.cpp
 * @Author:   fangsh
 * @email:    1447675994@qq.com
 * @data:     2023/01/03
 * @brief:   
 */

#include <stdio.h>
#include <signal.h>

#include "server_proc.h"
#include "util/systool.h"

#define USAGE_MSG   "Usage: %s  <config_file> \n" 

int main(int argc, char** argv) 
{
    if (argc < 2){
        fprintf(stderr, USAGE_MSG, argv[0]);
        return -1;
	}

	kkb::CSysTool::DaemonInit();

	signal(SIGPIPE, SIG_IGN);
	signal(SIGCHLD, SIG_IGN);

	CHttpSvrProc oProc;
	
	int iRet = oProc.Init(argv[1]);
	if(iRet != 0){
		fprintf(stderr, "http-server  init failed, ret=%d\n", iRet);
		return -1;
	}
	
	iRet = oProc.Run();
	if(iRet != 0){
		fprintf(stderr, "http-server  run failed, ret=%d\n", iRet);
		return -1;
	}else{
		fprintf(stderr, "http-server exit normal\n");
	}

	return 0;  
} 




