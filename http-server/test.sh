#########################################################################
# File Name:    test.sh
# Author:       fangsh
# mail:         1447675994@qq.com
# Created Time: 2023年01月02日 星期一 09时02分27秒
#########################################################################
#!/bin/bash


curl 127.0.0.1:8081/get_ver?product=1&guid=2&zone=3&ver=4


scp -r *  root@81.71.24.140:/data/code/github/libevent-module
