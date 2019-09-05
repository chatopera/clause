#! /bin/bash 
###########################################
#
###########################################

# constants
baseDir=$(cd `dirname "$0"`;pwd)
rootDir=$(cd $baseDir/../../..;pwd)
BUILDDIR=$rootDir/build/debug
PRJNAME=src/redis
TARGETNAME=redis_test
export GLOG_logtostderr=1
# export GLOG_minloglevel=2
export GLOG_v=3

# functions

# main 
[ -z "${BASH_SOURCE[0]}" -o "${BASH_SOURCE[0]}" = "$0" ] || return
cd $rootDir
if [ ! -d $BUILDDIR ]; then
    $rootDir/admin/generator.sh
fi

cd $BUILDDIR/$PRJNAME
make

if [ ! $? -eq 0 ]; then
    echo `pwd` 
    ls -al
    exit 1
fi

if [[ $# -eq 0 ]]; then
    echo "None pass in testcase, set as default."
    set -x
    ./$TARGETNAME --gtest_filter=RedisTest.INIT
elif [[ $# -eq 1 ]]; then
    echo "Pass-in test:" $1
    set -x
    if [[ "$1" = "ALL" ]]; then
        ./$TARGETNAME
    else
        ./$TARGETNAME --gtest_filter=RedisTest.$1
    fi
else 
    echo "Usage: $0 [TESTCASE_NAME|ALL]"
    echo "Example: $0 INIT"
    exit 1 
fi
