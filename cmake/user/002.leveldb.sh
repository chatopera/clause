#! /bin/bash 
###########################################
#
###########################################

# constants
baseDir=$(cd `dirname "$0"`;pwd)
ROOR_INSTALL_DIR=$baseDir/../..
THIRD_PARTY=$ROOR_INSTALL_DIR/thirdparty
OPTDIR=$ROOR_INSTALL_DIR/build/leveldb
SRCDIR=$THIRD_PARTY/leveldb
BUILD=build-Release
SUDO_CMD=""
export LD_LIBRARY_PATH=$ROOR_INSTALL_DIR/lib:$LD_LIBRARY_PATH

# functions

# main 
[ -z "${BASH_SOURCE[0]}" -o "${BASH_SOURCE[0]}" = "$0" ] || return
which sudo
if [ $? -eq 0 ]; then
    SUDO_CMD=sudo
fi

$SUDO_CMD rm -rf $OPTDIR
mkdir $OPTDIR; cd $OPTDIR
tar xzf $SRCDIR/leveldb.tgz

## compile
cd leveldb; rm -rf build
mkdir build; cd build
cmake -G "Unix Makefiles" \
    -DCMAKE_INSTALL_PREFIX=$ROOR_INSTALL_DIR \
    -DCMAKE_CXX_FLAGS="-D_GLIBCXX_USE_CXX11_ABI=0 -fPIC" \
    -DCMAKE_CXX_STANDARD=11 ..
make -j`nproc` && $SUDO_CMD make install
