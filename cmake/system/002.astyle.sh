#! /bin/bash 
###########################################
# Install pthread as a lib
###########################################

# constants
baseDir=$(cd `dirname "$0"`;pwd)
ROOR_INSTALL_DIR=$baseDir/../..
THIRD_PARTY=$ROOR_INSTALL_DIR/thirdparty
SRCDIR=$THIRD_PARTY/astyle
OPTDIR=$ROOR_INSTALL_DIR/build/astyle
BUILD=build-Release
SUDO_CMD=""

# functions

# main 
[ -z "${BASH_SOURCE[0]}" -o "${BASH_SOURCE[0]}" = "$0" ] || return
echo "Install pthread ..."

which sudo
if [ $? -eq 0 ]; then
    SUDO_CMD=sudo
fi

if [ -d $OPTDIR ]; then
    $SUDO_CMD rm -rf $OPTDIR
fi

cp -rf $SRCDIR $OPTDIR; cd $OPTDIR
mkdir  release
cd  release
cmake  ../
make -j${nproc}
$SUDO_CMD make install