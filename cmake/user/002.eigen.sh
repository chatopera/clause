#! /bin/bash 
###########################################
# Install jsoncpp as a lib
# https://github.com/open-source-parsers/jsoncpp/wiki/Building
###########################################

# constants
baseDir=$(cd `dirname "$0"`;pwd)
ROOR_INSTALL_DIR=$baseDir/../..
THIRD_PARTY=$ROOR_INSTALL_DIR/thirdparty
SRCDIR=$ROOR_INSTALL_DIR/thirdparty/eigen.tgz
OPTDIR=$ROOR_INSTALL_DIR/build/eigen
SUDO_CMD=""

# functions

# main 
[ -z "${BASH_SOURCE[0]}" -o "${BASH_SOURCE[0]}" = "$0" ] || return
echo "Install eigen ..."

which sudo
if [ $? -eq 0 ]; then
    SUDO_CMD=sudo
fi

$SUDO_CMD rm -rf $ROOR_INSTALL_DIR/include/Eigen
$SUDO_CMD rm -rf $OPTDIR
mkdir $OPTDIR; cd $OPTDIR

tar xzf $SRCDIR
$SUDO_CMD cp -rf eigen/include/* $ROOR_INSTALL_DIR/include
