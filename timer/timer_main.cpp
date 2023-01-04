

#include <stdio.h>
#include <signal.h>

#include "timer_proc.h"
#include "util/systerm_tool.h"

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

	CTimerProc oProc;
	
	int iRet = oProc.Init(argv[1]);
	if(iRet != 0){
		fprintf(stderr, "Error: Timer init failed, ret=%d\n", iRet);
		return -1;
	}
	
	iRet = oProc.Run();
	if(iRet != 0){
		fprintf(stderr, "Error: Timer run failed, ret=%d\n", iRet);
		return -1;
	}else{
		fprintf(stderr, "Info: Timer exit normal\n");
	}

	return 0;  
} 

