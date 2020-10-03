#! /bin/bash 
###########################################
#
###########################################

# constants
baseDir=$(cd `dirname "$0"`;pwd)
rootDir=$(cd  "$baseDir/../..";pwd)
buildDir=$rootDir/build/debug
tmpDir=$rootDir/tmp
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$rootDir/lib

# functions

# main 
ulimit -c 10240000000

#functions

# main 
[ -z "${BASH_SOURCE[0]}" -o "${BASH_SOURCE[0]}" = "$0" ] || return
if [ "$#" -ne 3 ]; then
    echo "Illegal number of parameters"
    echo "Usage: $0 PROJECT_DIR TARGET_NAME FLAG_FILE"
    echo "Example: $0 src/hpms hpms_server ./cmd.gflags" 
else
    # do some checks
    $baseDir/../precompile.sh
    source $rootDir/.env

    set -x
    PROJECT_DIR=$1
    TARGET_NAME=$2
    FLAG_FILE=$3
    BINARY_PATH=$buildDir/$PROJECT_DIR/$TARGET_NAME
    BINARY_DIR=`dirname $BINARY_PATH`
    SRC_PATH=$rootDir/$PROJECT_DIR
    LOGFILE=$tmpDir/$TARGET_NAME.root.log

    $baseDir/stop.sh $PROJECT_DIR $TARGET_NAME
    nohup $baseDir/start.sh $PROJECT_DIR  $TARGET_NAME $FLAG_FILE 2>&1 >> $LOGFILE &
fi