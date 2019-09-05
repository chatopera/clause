#! /bin/bash 
###########################################
#
###########################################

# constants
baseDir=$(cd `dirname "$0"`;pwd)
ROOR_INSTALL_DIR=$baseDir/../..
THIRD_PARTY=$ROOR_INSTALL_DIR/thirdparty
OPTDIR=$ROOR_INSTALL_DIR/build/crfsuite
SRCDIR=$THIRD_PARTY/crfsuite
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

$SUDO_CMD apt-get update -y &&
    $SUDO_CMD apt-get install -y liblbfgs-dev

$SUDO_CMD rm -rf $OPTDIR
mkdir $OPTDIR
cd $SRCDIR/crfsuite-0.12; tar cf - .|(cd $OPTDIR; tar xf -)

## compile
cd $OPTDIR
./configure \
    CFLAGS="-D_GLIBCXX_USE_CXX11_ABI=0 -fPIC" \
    CPPFLAGS="-D_GLIBCXX_USE_CXX11_ABI=0 -fPIC" \
    CXXFLAGS="-D_GLIBCXX_USE_CXX11_ABI=0 -fPIC" \
    --prefix=$ROOR_INSTALL_DIR \
    --bindir=$ROOR_INSTALL_DIR/bin
make
$SUDO_CMD make install

# has modified these files in src/ner/src
$SUDO_CMD rm -rf $ROOR_INSTALL_DIR/include/crfsuite_api.hpp
$SUDO_CMD rm -rf $ROOR_INSTALL_DIR/include/crfsuite.hpp