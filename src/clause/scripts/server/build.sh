#! /bin/bash 
###########################################
#
###########################################

# constants
baseDir=$(cd `dirname "$0"`;pwd)
rootDir=$(cd $baseDir/../../../..;pwd)
imagename=chatopera/clause
releasebundle=$baseDir/../../tmp/app
releaselib=$baseDir/../../tmp/app/lib
releasebinary=$rootDir/build/debug/src/clause/clause_server

# functions

# main 
[ -z "${BASH_SOURCE[0]}" -o "${BASH_SOURCE[0]}" = "$0" ] || return
cd $rootDir
imageversion=`git rev-parse --short HEAD`

if [ ! -d $baseDir/../../tmp ]; then
    mkdir -p $baseDir/../../tmp
fi

if [ -d $releasebundle ]; then
    rm -rf $releasebundle
fi
mkdir -p $releasebundle

echo "1. copy libraries ..."
cp -rf $rootDir/lib $releaselib
cp /usr/lib/libevent-2.1.so.6 $releaselib
if [ -d $releaselib/paddle ];then
    rm -rf $releaselib/paddle
fi

echo "2. delete useless libs ..."
rm -rf $releaselib/*.{la,a}
rm -rf $releaselib/cmake
# TODO turn off for production
# for debugging usage
# rm -rf $releaselib/libtcmalloc*
# rm -rf $releaselib/libprofiler*

echo "3. check bundled libraries ..."
libname=libgflags.so.2.2
if [ ! -f $releaselib/$libname ]; then
    echo "[fatal] $libname lib not exist!"
    exit 1
fi

libname=libglog.so.0
if [ ! -f $releaselib/$libname ]; then
    echo "[fatal] $libname lib not exist!"
    exit 1
fi

libname=libthriftnb-0.12.0.so
if [ ! -f $releaselib/$libname ]; then
    echo "[fatal] $libname lib not exist!"
    exit 1
fi

libname=libmysqlcppconn.so.7
if [ ! -f $releaselib/$libname ]; then
    echo "[fatal] $libname lib not exist!"
    exit 1
fi

libname=libevent-2.1.so.6
if [ ! -f $releaselib/$libname ]; then
    echo "[fatal] $libname lib not exist!"
    exit 1
fi

libname=libprotobuf.so.20.0.1
if [ ! -f $releaselib/$libname ]; then
    echo "[fatal] $libname lib not exist!"
    exit 1
fi

libname=x86_64-linux-gnu/libjsoncpp.so
if [ ! -f $releaselib/$libname ]; then
    echo "[fatal] $libname lib not exist!"
    exit 1
fi

## copy binary
echo "4. copy binary ..."
if [ -f $releasebinary ]; then
    cp $releasebinary $releasebundle
else
    echo "Binary not exist!"
fi

cd $baseDir/../..

echo "5. build docker image ..."
docker build \
    --build-arg VCS_REF=`git rev-parse --short HEAD` \
    --no-cache=true \
    --force-rm=true --tag $imagename:$imageversion .
docker tag $imagename:$imageversion $imagename:develop

#echo "6. push docker image ..."
#set -x
#docker push $imagename:$imageversion
#docker push $imagename:develop