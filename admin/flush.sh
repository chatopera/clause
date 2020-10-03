#! /bin/bash 
###########################################
#
###########################################

# constants
baseDir=$(cd `dirname "$0"`;pwd)
rootDir=$(cd $baseDir/..;pwd)
SUDO_CMD=""

# functions

# main 
[ -z "${BASH_SOURCE[0]}" -o "${BASH_SOURCE[0]}" = "$0" ] || return
which sudo
if [ $? -eq 0 ]; then
    SUDO_CMD=sudo
fi

cd $rootDir
echo "[WARN] wait for 10 seconds before clean up all data, be sure of what you are doing ..."
sleep 10
set -x
docker-compose stop
docker-compose rm --force

$SUDO_CMD rm -rf var/activemq/data/*
$SUDO_CMD rm -rf var/mysql/data/*
$SUDO_CMD rm -rf var/redis/data/*

