#! /bin/bash 
###########################################
# Install activemq cpp as a lib
###########################################

# constants
baseDir=$(cd `dirname "$0"`;pwd)
ROOR_INSTALL_DIR=$baseDir/../..
THIRDPARTY=$ROOR_INSTALL_DIR/thirdparty
OPTDIR=$ROOR_INSTALL_DIR/build/activemq
SUDO_CMD=""

# functions

# main 
[ -z "${BASH_SOURCE[0]}" -o "${BASH_SOURCE[0]}" = "$0" ] || return
which sudo
if [ $? -eq 0 ]; then
    SUDO_CMD=sudo
fi

if [ -f $THIRDPARTY/activemq/activemq.tgz ]; then
    echo "Install activemq cpp ..."
else
    echo $THIRDPARTY/activemq/activemq.tgz "does not exist."
    exit 1
fi

set -x
$SUDO_CMD apt-get update -y
$SUDO_CMD apt-get install -yy libcppunit-dev \
    libapr1 libapr1-dev openssl libssl-dev \
    automake libtool

rm -rf $OPTDIR
cd $ROOR_INSTALL_DIR/build; tar xzf $THIRDPARTY/activemq/activemq.tgz
cd $OPTDIR

./configure --prefix=$ROOR_INSTALL_DIR \
    CFLAGS="-std=c++11 -D_GLIBCXX_USE_CXX11_ABI=0 -fPIC" \
    CPPFLAGS="-std=c++11 -D_GLIBCXX_USE_CXX11_ABI=0 -fPIC" \
    CXXFLAGS="-std=c++11 -D_GLIBCXX_USE_CXX11_ABI=0 -fPIC"

make -j`nproc`
$SUDO_CMD make install
rm -rf $OPTDIR

echo "done."
