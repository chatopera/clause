#! /bin/bash 
###########################################
#
###########################################

# constants
baseDir=$(cd `dirname "$0"`;pwd)
ROOR_INSTALL_DIR=$baseDir/../..
THIRD_PARTY=$ROOR_INSTALL_DIR/thirdparty
OPTDIR=$ROOR_INSTALL_DIR/build/gflags
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

$SUDO_CMD dpkg -P libgflags-dev libgflags2v5

$SUDO_CMD rm -rf $OPTDIR
# Note: only built shared lib
if [ -d $THIRD_PARTY/gflags ]; then
    echo "Install gflags ..."
    cp -rf $THIRD_PARTY/gflags $OPTDIR
    cd $OPTDIR
    rm -rf $BUILD
    mkdir $BUILD && cd $BUILD
    nprocs=`cat /proc/cpuinfo| grep "processor"| wc -l`
    cmake -G "Unix Makefiles" \
        -DBUILD_STATIC_LIBS=0 \
        -DBUILD_SHARED_LIBS=1 \
        -DBUILD_TESTING=0 \
        -DGFLAGS_NAMESPACE=gflags \
        -DCMAKE_CXX_FLAGS="-D_GLIBCXX_USE_CXX11_ABI=0 -fPIC" \
        -DCMAKE_INSTALL_PREFIX=$ROOR_INSTALL_DIR  ..
    make -j${nprocs} && $SUDO_CMD make install
    $SUDO_CMD rm -rf $OPTDIR
    echo "Done with GFlag Installation."
else
    echo "Can not find gflags."
    exit 1
fi
