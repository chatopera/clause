#! /bin/bash 
###########################################
#
###########################################

# constants
baseDir=$(cd `dirname "$0"`;pwd)
ROOR_INSTALL_DIR=$baseDir/../..
THIRD_PARTY=$ROOR_INSTALL_DIR/thirdparty
OPTDIR=$ROOR_INSTALL_DIR/build/cmake
SRCDIR=$THIRD_PARTY/cmake
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

# remove old cmake
$SUDO_CMD dpkg -P cmake

$SUDO_CMD rm -rf $OPTDIR
mkdir $OPTDIR; cd $OPTDIR
tar xzf $SRCDIR/cmake-3.15.2.tar.gz

## compile
cd cmake-3.15.2
./configure --prefix=/usr/local
make
$SUDO_CMD make install
