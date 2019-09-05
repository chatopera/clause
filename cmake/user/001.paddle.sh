#! /bin/bash 
###########################################
# Install BAIDU LAC Dist packages
###########################################

# constants
baseDir=$(cd `dirname "$0"`;pwd)
ROOT_INSTALL_DIR=$baseDir/../..
THIRD_PARTY=$ROOT_INSTALL_DIR/thirdparty
OPTDIR=$ROOT_INSTALL_DIR/build/lac
SRCDIR=$THIRD_PARTY/lac
BUILD=build-Release
SUDO_CMD=""

# functions

# main 
[ -z "${BASH_SOURCE[0]}" -o "${BASH_SOURCE[0]}" = "$0" ] || return
which sudo
if [ $? -eq 0 ]; then
    SUDO_CMD=sudo
fi

# remove files
$SUDO_CMD rm -rf $ROOT_INSTALL_DIR/lib/paddle

$SUDO_CMD rm -rf $OPTDIR
mkdir $OPTDIR; cd $OPTDIR
tar xzf $SRCDIR/paddle.0.14.0.tgz
tar xzf $SRCDIR/lac.conf.tgz

cp -rf paddle $ROOT_INSTALL_DIR/lib/paddle
cp -rf conf $ROOT_INSTALL_DIR/var/test/lac