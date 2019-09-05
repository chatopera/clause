#! /bin/bash 
###########################################
#
###########################################

# constants
baseDir=$(cd `dirname "$0"`;pwd)

# functions

# main
[ -z "${BASH_SOURCE[0]}" -o "${BASH_SOURCE[0]}" = "$0" ] || return
cd $baseDir/.. && \
    rm -rf build/debug/ && \
    mkdir build/debug/ && \
    cd build/debug/ && \
    cmake ../..

# do some checks after creating profile
$baseDir/precompile.sh

pwd
