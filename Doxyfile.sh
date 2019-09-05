#! /bin/bash 
###########################################
#
###########################################

# constants
baseDir=$(cd `dirname "$0"`;pwd)
TARGET=$baseDir/docs
TMPDIR=$baseDir/tmp
# functions

# main 
[ -z "${BASH_SOURCE[0]}" -o "${BASH_SOURCE[0]}" = "$0" ] || return

if [ ! -d $TMPDIR ]; then
    mkdir $TMPDIR
fi

if [ ! -d $baseDir/docs ]; then
    mkdir $baseDir/docs
else
    rm -rf $baseDir/docs/*
fi

if [ -d $baseDir/tmp/doxygen_out ]; then
    rm -rf $baseDir/tmp/doxygen_out
fi

cd $baseDir
echo "Generating docs ..."
set -x
doxygen

if [ -d $baseDir/tmp/doxygen_out/html ]; then
    cd $baseDir/tmp/doxygen_out/html
    tar cf - .|(cd $TARGET; tar xf -)
    cp $baseDir/var/assets/doxygen.index.html $baseDir/docs/index.html
    cp -rf $baseDir/var/assets/js $baseDir/docs
    echo "built docs in" $TARGET
else
    echo "Docs not generated ."
    exit 1
fi
