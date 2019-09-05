#! /bin/bash 
###########################################
#
###########################################

# constants
baseDir=$(cd `dirname "$0"`;pwd)
ROOR_INSTALL_DIR=$baseDir/../..
OPTDIR=$ROOR_INSTALL_DIR/build/glog
THIRD_PARTY=$ROOR_INSTALL_DIR/thirdparty
SUDO_CMD=""
export LD_LIBRARY_PATH=$ROOR_INSTALL_DIR/lib:$LD_LIBRARY_PATH

# functions

# main 
[ -z "${BASH_SOURCE[0]}" -o "${BASH_SOURCE[0]}" = "$0" ] || return
which sudo
if [ $? -eq 0 ]; then
    SUDO_CMD=sudo
fi

$SUDO_CMD apt-get install -y autoconf automake libtool

# Note: only built shared lib
if [ -d $THIRD_PARTY/glog ]; then
    echo "Install glog ..."
    $SUDO_CMD rm -rf $OPTDIR
    cp -rf $THIRD_PARTY/glog $OPTDIR
    cd $OPTDIR
    ./autogen.sh
    ./configure CPPFLAGS="-D_GLIBCXX_USE_CXX11_ABI=0 -DGFLAGS_NAMESPACE=gflags" \
        --prefix=$ROOR_INSTALL_DIR \
        --with-gflags=$ROOR_INSTALL_DIR \
        -with-pic
    nprocs=`cat /proc/cpuinfo| grep "processor"| wc -l`
    make -j${nprocs}
    $SUDO_CMD make install
    $SUDO_CMD make clean
    $SUDO_CMD rm -rf $OPTDIR
    $SUDO_CMD rm -rf $ROOR_INSTALL_DIR/lib/libglog.a
    $SUDO_CMD rm -rf $ROOR_INSTALL_DIR/lib/libglog.la
    echo "Done with Glog Installation."
else
    echo "Can not find glog."
    exit 1
fi
