#! /bin/bash 
###########################################
#
###########################################

# constants
baseDir=$(cd `dirname "$0"`;pwd)
ROOR_INSTALL_DIR=$baseDir/../..
OPTDIR=$ROOR_INSTALL_DIR/build/googletest
THIRD_PARTY=$ROOR_INSTALL_DIR/thirdparty
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
if [ -d $THIRD_PARTY/googletest ]; then
    echo "Install googletest ..."
    $SUDO_CMD rm -rf $OPTDIR
    cp -rf $THIRD_PARTY/googletest $OPTDIR
    cd $OPTDIR
    if [ -d $BUILD ]; then
        rm -rf $BUILD
    fi
    mkdir $BUILD && cd $BUILD
    nprocs=`cat /proc/cpuinfo| grep "processor"| wc -l`
    cmake -G "Unix Makefiles" \
        -DCMAKE_INSTALL_PREFIX=$ROOR_INSTALL_DIR \
        -DCMAKE_CXX_FLAGS="-D_GLIBCXX_USE_CXX11_ABI=0 -fPIC" \
        -DCMAKE_CXX_STANDARD=11 ..
    make -j${nprocs} && $SUDO_CMD make install
    echo "Done with Google Test Installation."
else
    echo "Can not find googletest."
    exit 1
fi
