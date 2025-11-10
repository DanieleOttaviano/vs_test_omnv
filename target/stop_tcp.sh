#!/bin/bash

tcp_pid=$(ps aux | grep '[t]cpdump' | awk '{print $1}')
echo "tcpdump PID: $tcp_pid"
kill -SIGINT $tcp_pid
