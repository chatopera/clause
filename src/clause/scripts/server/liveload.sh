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
# functions

# main
[ -z "${BASH_SOURCE[0]}" -o "${BASH_SOURCE[0]}" = "$0" ] || return
cd $rootDir/admin/dev
./liveload.sh $PROJECT_DIR $TARGET_NAME $FLAG_FILE

