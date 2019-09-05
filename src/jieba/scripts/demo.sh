#! /bin/bash 
###########################################
#
###########################################

# constants
baseDir=$(cd `dirname "$0"`;pwd)
rootDir=$(cd $baseDir/../../..;pwd)

# functions

# main 
[ -z "${BASH_SOURCE[0]}" -o "${BASH_SOURCE[0]}" = "$0" ] || return
cd $rootDir

if [ ! -d build/debug ]; then
    ./admin/generator.sh
fi

cd build/debug/src/jieba
make jiebademo
if [ $? -eq 0 ]; then
    ./jiebademo
fi


