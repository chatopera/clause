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
ulimit -c unlimited

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

    echo "start application ..."
    # set -x
    PROJECT_DIR=$1
    TARGET_NAME=$2
    FLAG_FILE=$3
    BINARY_PATH=$buildDir/$PROJECT_DIR/$TARGET_NAME
    BINARY_DIR=`dirname $BINARY_PATH`
    SRC_PATH=$rootDir/$PROJECT_DIR
    LOGFILE=$tmpDir/$TARGET_NAME.root.log
    if [ ! -d $SRC_PATH ]; then
        echo "source code folder" $SRC_PATH "not exist. "
        exit 1
    fi

    if [ ! -f $FLAG_FILE ]; then
        echo "Flag file does not exist, find sample file in $SRC_PATH" 
        exit 1
    fi

    cd $BINARY_DIR
    ./$TARGET_NAME --flagfile=$FLAG_FILE
fi
