#! /bin/bash 
###########################################
#
###########################################

# constants
baseDir=$(cd `dirname "$0"`;pwd)
rootDir=$(cd $baseDir/../../../..;pwd)
build=$rootDir/build/debug
PROJECT_DIR=src/clause
TARGET_NAME=clause_server
FLAG_FILE=$baseDir/../../config/server/cmd.flags
COMPILE_CMD=$baseDir/compile.sh

# functions

# main
[ -z "${BASH_SOURCE[0]}" -o "${BASH_SOURCE[0]}" = "$0" ] || return
ulimit -c unlimited

$COMPILE_CMD
if [ $? -eq 0 ]; then
    cd $rootDir/admin/dev
    ./start.sh $PROJECT_DIR $TARGET_NAME $FLAG_FILE
else
    echo "Compiling failure."
    exit 1
fi



