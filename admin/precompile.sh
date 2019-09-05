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
if [ ! -f $baseDir/localrc ]; then
    cp $baseDir/localrc.sample $baseDir/localrc
    echo "[WARN] Check localrc in " $baseDir/localrc
fi

if [ ! -d $buildDir ]; then
    $baseDir/generator.sh
fi

if [ ! -d $baseDir/../tmp ]; then
    mkdir $baseDir/../tmp
fi
