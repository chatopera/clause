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
[ -z "${BASH_SOURCE[0]}" -o "${BASH_SOURCE[0]}" = "$0" ] || return
echo $rootDir
if [ "$#" -ne 2 ]; then
    echo "Illegal number of parameters"
    echo "Usage: $0 PROJECT_DIR TARGET_NAME FLAG_FILE"
    echo "Example: $0 src/hpms hpms_server" 
else
    # do some checks
    $rootDir/admin/precompile.sh
    source $rootDir/admin/localrc

    set -x
    PROJECT_DIR=$1
    TARGET_NAME=$2
    BINARY_PATH=$buildDir/$PROJECT_DIR/$TARGET_NAME
    BINARY_DIR=`dirname $BINARY_PATH`
    SRC_PATH=$rootDir/$PROJECT_DIR
    LOGFILE=$tmpDir/$TARGET_NAME.root.log

    if [ ! -d $SRC_PATH ]; then
        echo "source code folder" $SRC_PATH "not exist. "
        exit 1
    fi

    if [ -f $BINARY_PATH ]; then
        rm -rf $BINARY_PATH
    fi

    cd $buildDir/$PROJECT_DIR
    make -j`nproc` $TARGET_NAME 2>&1 > $LOGFILE 
    if [ ! $? -eq 0 ]; then
        exit 1
    fi
fi
