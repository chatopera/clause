#! /bin/bash 
###########################################
#
###########################################

# constants
baseDir=$(cd `dirname "$0"`;pwd)
ROOR_INSTALL_DIR=$baseDir/../..
THIRD_PARTY=$ROOR_INSTALL_DIR/thirdparty
OPTDIR=$ROOR_INSTALL_DIR/build/gperftools
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

if [ -d $OPTDIR ]; then
    rm -rf $OPTDIR
fi
mkdir $OPTDIR; cd $OPTDIR
tar xzf $THIRD_PARTY/gperftools/gperftools-2.7.tar.gz

cd gperftools-2.7
./configure --disable-static \
    CFLAGS="-D_GLIBCXX_USE_CXX11_ABI=0 -fPIC" \
    CPPFLAGS="-D_GLIBCXX_USE_CXX11_ABI=0 -fPIC" \
    CXXFLAGS="-D_GLIBCXX_USE_CXX11_ABI=0 -fPIC" \
    --prefix=$ROOR_INSTALL_DIR
make
$SUDO_CMD make install
