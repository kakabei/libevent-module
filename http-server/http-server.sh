#!/bin/sh

start()
{
    ./http-server ./http-server.ini
}

stop()
{
    killall http-server
}

usage()
{
    echo "$0 <start|stop|restart>"
}

if [ "$1" = "start" ];then
    start

elif [ "$1" = "stop" ];then
    stop

elif [ "$1" = "restart" ];then
    stop
    start
else
    usage
fi

