#! /bin/bash 
###########################################
# do some checks before compile
###########################################

# constants
baseDir=$(cd `dirname "$0"`;pwd)
buildDir=$baseDir/../build/debug

# functions

# main
[ -z "${BASH_SOURCE[0]}" -o "${BASH_SOURCE[0]}" = "$0" ] || return
if [ ! -f $baseDir/../.env ]; then
    cp $baseDir/../sample.env $baseDir/../.env
    echo "[WARN] Check .env in " $baseDir/..
    exit 1
fi

if [ ! -d $buildDir ]; then
    $baseDir/generator.sh
fi

if [ ! -d $baseDir/../tmp ]; then
    mkdir $baseDir/../tmp
fi
