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
cd $rootDir
if [ ! -d $BUILDDIR ]; then
    $rootDir/admin/generator.sh
fi

cd $BUILDDIR/src/hat-trie
make

if [ ! $? -eq 0 ]; then
    echo `pwd` 
    ls -al
    exit 1
fi

if [[ $# -eq 0 ]]; then
    echo "None pass in testcase, set as default."
    set -x
    ./hattrie_test --gtest_filter=HattrieTest.INIT
elif [[ $# -eq 1 ]]; then
    echo "Pass-in test:" $1
    set -x
    if [[ "$1" = "ALL" ]]; then
        ./hattrie_test
    else
        ./hattrie_test --gtest_filter=HattrieTest.$1
    fi
else 
    echo "Usage: $0 [TESTCASE_NAME|ALL]"
    echo "Example: $0 ADD_TERMS"
    exit 1 
fi
