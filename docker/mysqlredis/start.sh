#!/bin/bash

# 启动 MySQL
docker-entrypoint.sh mysqld &

# 启动 Redis
redis-server --requirepass 123456 --daemonize yes

# 防止容器退出
tail -f /dev/null