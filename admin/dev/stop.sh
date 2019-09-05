#! /bin/bash 
###########################################
#
###########################################

# constants
baseDir=$(cd `dirname "$0"`;pwd)
rootDir=$(cd "$baseDir/../..";pwd)
buildDir=$rootDir/build/debug
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$rootDir/lib

# functions

# main 
[ -z "${BASH_SOURCE[0]}" -o "${BASH_SOURCE[0]}" = "$0" ] || return
if [ "$#" -ne 2 ]; then
    echo "Illegal number of parameters"
    echo "Usage: $0 PROJECT_DIR TARGET_NAME"
    echo "Example: $0 src/hpms hpms_server" 
else
    echo "stop application ..."
    # set -x
    PROJECT_DIR=$1
    TARGET_NAME=$2
    BINARY_PATH=$buildDir/$PROJECT_DIR/$TARGET_NAME
    BINARY_DIR=`dirname $BINARY_PATH`
    SRC_PATH=$rootDir/$PROJECT_DIR

    if [ ! -d $SRC_PATH ]; then
        echo "source code folder" $SRC_PATH "not exist. "
        exit 1
    fi

    for x in `ps -ef|grep "$BINARY_PATH"|awk '{ print $2 }'`; do
        ps -p $x 2>&1 >>/dev/null
        if [ $? == 0 ];then
            sudo kill -9 $x 2>&1 >>/dev/null ;
        fi
    done
fi

