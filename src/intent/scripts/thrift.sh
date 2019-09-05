#! /bin/bash 
###########################################
#
###########################################

# constants
baseDir=$(cd `dirname "$0"`;pwd)
rootDir=$(cd $baseDir/../../..;pwd)
genDir=$baseDir/../serving
clientDir=$baseDir/../client
export PATH=$PATH:$rootDir/bin
# functions

# main 
[ -z "${BASH_SOURCE[0]}" -o "${BASH_SOURCE[0]}" = "$0" ] || return
cd $baseDir/../thrift

## generate cpp serving
rm -rf $genDir
mkdir $genDir
thrift --gen cpp -out $genDir server.dsl
# rm -rf $genDir/*.skeleton.cpp

## generate nodejs client for testing & integrating
cd $baseDir/../thrift

if [ ! -d $clientDir ]; then
    mkdir -p $clientDir
fi

rm -rf $clientDir/gen-nodejs
thrift --gen js:node -o $clientDir server.dsl

if [ $? -eq 0 ]; then
    rm -rf $clientDir/node_modules
fi