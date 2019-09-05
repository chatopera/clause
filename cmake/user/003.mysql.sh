#! /bin/bash 
###########################################
# Install jsoncpp as a lib
# https://dev.mysql.com/doc/connector-cpp/8.0/en/
###########################################

# constants
baseDir=$(cd `dirname "$0"`;pwd)
ROOR_INSTALL_DIR=$baseDir/../..
THIRD_PARTY=$ROOR_INSTALL_DIR/thirdparty
SRCDIR=$ROOR_INSTALL_DIR/thirdparty/mysql
OPTDIR=$ROOR_INSTALL_DIR/build/mysql
SUDO_CMD=""

# functions

# main 
[ -z "${BASH_SOURCE[0]}" -o "${BASH_SOURCE[0]}" = "$0" ] || return
echo "Install mysql drivers ..."

which sudo
if [ $? -eq 0 ]; then
    SUDO_CMD=sudo
fi

echo "Install mysql as dynamic lib ..."

if [ -d $OPTDIR ]; then
    $SUDO_CMD rm -rf $OPTDIR
fi

$SUDO_CMD apt-get update && $SUDO_CMD apt-get install -y uuid-dev

$SUDO_CMD rm -rf $ROOR_INSTALL_DIR/include/mysql
$SUDO_CMD rm -rf $ROOR_INSTALL_DIR/lib/libmysql*

mkdir $OPTDIR && cd $OPTDIR
tar xzf $SRCDIR/assets/mysql-connector-c++-8.0.17-linux-glibc2.12-x86-64bit.tar.gz
cd mysql-connector-c++-8.0.17-linux-glibc2.12-x86-64bit
$SUDO_CMD cp -rf include $ROOR_INSTALL_DIR/include/mysql
$SUDO_CMD cp -rf lib64/lib* $ROOR_INSTALL_DIR/lib

## fix thrift compiler build issue with bundled openssl libs
$SUDO_CMD rm -rf $ROOR_INSTALL_DIR/lib/libcrypto.so*
$SUDO_CMD rm -rf $ROOR_INSTALL_DIR/lib/libssl.so*