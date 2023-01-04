/** 
 * @file:     systool.cpp
 * @Author:   fangsh
 * @email:    1447675994@qq.com
 * @data:     2023/01/03
 * @brief:   
 */

#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <iconv.h>
#include <limits.h>
#include <sstream>

#include "systerm_tool.h"
	
using namespace std;
using namespace kkb;

int CSysTool::DaemonInit()
{

	pid_t pid = fork();
		
	if (pid == -1){
		return E_SYS_TOOL_FORK;
    } else if (pid != 0){ // Parent exits.
		exit(0);
	}	
    // 1st child continues.
    setsid(); // Become session leader.
    signal(SIGHUP, SIG_IGN);

    pid = fork();
    if (pid != 0) {
		exit(0); // First child terminates.
	}
	// clear our file mode creation mask.
    umask(0);
    return SUCCESS;
}
