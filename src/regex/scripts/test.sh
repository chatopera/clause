#! /bin/bash 
###########################################
#
###########################################

# constants
baseDir=$(cd `dirname "$0"`;pwd)
rootDir=$(cd $baseDir/../../..;pwd)
BUILDDIR=$rootDir/build/debug
export GLOG_logtostderr=1
# export GLOG_minloglevel=2
export GLOG_v=4

# functions

# main 
[ -z "${BASH_SOURCE[0]}" -o "${BASH_SOURCE[0]}" = "$0" ] || return
ulimit -c unlimited

cd $rootDir
if [ ! -d $BUILDDIR ]; then
    $rootDir/admin/generator.sh
fi

cd $BUILDDIR/src/regex
make

if [ ! $? -eq 0 ]; then
    echo `pwd` 
    ls -al
    exit 1
fi

if [[ $# -eq 0 ]]; then
    echo "None pass in testcase, set as default."
    set -x
    ./regex_test --gtest_filter=RegexTest.INIT
elif [[ $# -eq 1 ]]; then
    echo "Pass-in test:" $1
    set -x
    if [[ "$1" = "ALL" ]]; then
        ./regex_test
    else
        ./regex_test --gtest_filter=RegexTest.$1
    fi
else 
    echo "Usage: $0 [TESTCASE_NAME|ALL]"
    echo "Example: $0 ADD_TERMS"
    exit 1 
fi
