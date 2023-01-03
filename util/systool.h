/** 
 * @file:     systool.h
 * @Author:   fangsh
 * @email:    1447675994@qq.com
 * @data:     2023年01月03日 星期二 14时18分10秒
 * @brief:   
 */

#include <stdio.h>
#include <stdarg.h>
#include <string>
#include <vector>
#include <inttypes.h>
#include <sys/time.h>
#include <set>

namespace kkb
{


class CSysTool
{

public:
	const static int SUCCESS = 0;
    const static int ERROR   = -1;

    const static int E_SYS_TOOL_MODULE_NAME		= -1001;
    const static int E_SYS_TOOL_OPEN_FILE		= -1002;
    const static int E_SYS_TOOL_LOCK_FILE       = -1003;
    const static int E_SYS_TOOL_FORK            = -1004;


     static int DaemonInit();
									
};
} // namespace kkb 
