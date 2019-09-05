#! /bin/bash 
###########################################
#
###########################################

# constants
baseDir=$(cd `dirname "$0"`;pwd)
rootDir=$(cd $baseDir/../../..;pwd)
PRJNAME=src/ner
SRCDIR=$rootDir/$PRJNAME
TARDIR=$rootDir/build/debug/$PRJNAME
TARNAME=crfsuitedemo
FLAGFILE=$SRCDIR/assets/cmd.flags.sample
export GLOG_logtostderr=1
# export GLOG_minloglevel=2
export GLOG_v=3

# functions

# main 
[ -z "${BASH_SOURCE[0]}" -o "${BASH_SOURCE[0]}" = "$0" ] || return
if [ ! -d $rootDir/build/debug ]; then
    $rootDir/admin/generator.sh
fi

if [ ! -d $rootDir/tmp ]; then
    mkdir $rootDir/tmp
fi

set -x
cd $TARDIR
make
if [ $? -eq 0 ]; then
    ./$TARNAME --flagfile=$FLAGFILE
fi