#! /bin/bash 
###########################################
#
###########################################

# constants
baseDir=$(cd `dirname "$0"`;pwd)
buildType=Debug

# functions

# main
[ -z "${BASH_SOURCE[0]}" -o "${BASH_SOURCE[0]}" = "$0" ] || return
if [ -f $baseDir/../build/released ]; then
    buildType=Release
    echo "Generate target files for release ..."
fi

set -x
cd $baseDir/.. && \
    rm -rf build/debug/ && \
    mkdir build/debug/ && \
    cd build/debug/ && \
    cmake -DCMAKE_BUILD_TYPE=$buildType ../..

# do some checks after creating profile
$baseDir/precompile.sh

pwd
