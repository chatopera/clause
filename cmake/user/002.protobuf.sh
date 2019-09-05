#! /bin/bash 
###########################################
#
###########################################

# constants
baseDir=$(cd `dirname "$0"`;pwd)
ROOR_INSTALL_DIR=$baseDir/../..
THIRD_PARTY=$ROOR_INSTALL_DIR/thirdparty
OPTDIR=$ROOR_INSTALL_DIR/build/protobuf
SRCFILE=$THIRD_PARTY/protobuf/protobuf-cpp-3.9.1.tar.gz
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

# remove old versions
$SUDO_CMD sudo apt-get remove -y libprotobuf-dev protobuf-compiler
$SUDO_CMD rm -rf /usr/local/lib/libprotobuf.*
$SUDO_CMD rm -rf $ROOR_INSTALL_DIR/lib/libprotobuf.*
$SUDO_CMD rm -rf $ROOR_INSTALL_DIR/bin/protoc
$SUDO_CMD rm -rf $ROOR_INSTALL_DIR/bin/.libs
if [ -d $ROOR_INSTALL_DIR/include/google/protobuf ]; then
    $SUDO_CMD rm -rf $ROOR_INSTALL_DIR/include/google/protobuf
fi
$SUDO_CMD rm -rf $OPTDIR

if [ -f $SRCFILE ]; then
    echo "Install protobuf ..."
    mkdir $OPTDIR; cd $OPTDIR
    tar xzf $SRCFILE
    cd protobuf-3.9.1
    $SUDO_CMD autoreconf -ivf
    ./autogen.sh
    ./configure \
        CFLAGS="-std=gnu++11 -D_GLIBCXX_USE_CXX11_ABI=0 -fPIC" \
        CPPFLAGS="-std=gnu++11 -D_GLIBCXX_USE_CXX11_ABI=0 -fPIC" \
        CXXFLAGS="-std=gnu++11 -D_GLIBCXX_USE_CXX11_ABI=0 -fPIC" \
        --prefix=$ROOR_INSTALL_DIR
    make -j`nproc` && $SUDO_CMD make install
    $SUDO_CMD ln -s  `pwd`/src/protoc $ROOR_INSTALL_DIR/bin
    
    echo "Done with protobuf Installation."
else
    echo "Can not find protobuf in thirdparty."
    exit 1
fi
