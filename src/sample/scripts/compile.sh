#! /bin/bash 
###########################################
#
###########################################

# constants
baseDir=$(cd `dirname "$0"`;pwd)
rootDir=$(cd $baseDir/../../..;pwd)
build=$rootDir/build/debug
project=src/sample

# functions

# main
[ -z "${BASH_SOURCE[0]}" -o "${BASH_SOURCE[0]}" = "$0" ] || return
if [ -d $build ]; then
    cd $build/$project
    # make clean
    make -j`nproc`
else 
    echo "Fail to resolve debug dir, run " $rootDir/admin/generator.sh " first."
    exit 1
fi

