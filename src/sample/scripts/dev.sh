#! /bin/bash 
###########################################
#
###########################################

# constants
baseDir=$(cd `dirname "$0"`;pwd)
rootDir=$(cd $baseDir/../../..;pwd)
build=$rootDir/build/debug
project=src/sample
binname=sample

# functions

# main
[ -z "${BASH_SOURCE[0]}" -o "${BASH_SOURCE[0]}" = "$0" ] || return
if [ ! -f $rootDir/admin/localrc ]; then
    cp $rootDir/admin/localrc.sample $rootDir/admin/localrc
fi
source $rootDir/admin/localrc

cd $baseDir
./compile.sh

if [ $? -eq 0 ]; then
    cd $build/$project
    ./$binname -v 3
fi