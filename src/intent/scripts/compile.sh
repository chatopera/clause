#! /bin/bash 
###########################################
#
###########################################

# constants
baseDir=$(cd `dirname "$0"`;pwd)
rootDir=$(cd $baseDir/../../..;pwd)
build=$rootDir/build/debug
PROJECT_DIR=src/intent
TARGET_NAME=intent_server

# functions

# main
[ -z "${BASH_SOURCE[0]}" -o "${BASH_SOURCE[0]}" = "$0" ] || return
cd $rootDir/admin/dev
./compile.sh $PROJECT_DIR $TARGET_NAME

if [ $? -eq 0 ]; then
    echo "Compile done."
else
    echo "Compile error."
    exit 1
fi