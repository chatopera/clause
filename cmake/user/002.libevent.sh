#! /bin/bash 
###########################################
#
###########################################

# constants
baseDir=$(cd `dirname "$0"`;pwd)
ROOR_INSTALL_DIR=$baseDir/../..
OPTDIR=$ROOR_INSTALL_DIR/build/libevent
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
$SUDO_CMD apt-get remove --purge -y '^libevent.*'
$SUDO_CMD rm -rf /usr/lib/libevent*

wget https://github.com/libevent/libevent/releases/download/release-2.1.10-stable/libevent-2.1.10-stable.tar.gz
tar xzf libevent-2.1.10-stable.tar.gz
cd libevent-2.1.10-stable
./autogen.sh
./configure \
    --disable-openssl \
    --enable-static=yes \
    --enable-shared=yes \
    CFLAGS="-D_GLIBCXX_USE_CXX11_ABI=0 -fPIC" \
    CPPFLAGS="-D_GLIBCXX_USE_CXX11_ABI=0 -fPIC" \
    CXXFLAGS="-D_GLIBCXX_USE_CXX11_ABI=0 -fPIC" \
    --prefix=/usr
    
make
$SUDO_CMD make install
$SUDO_CMD rm -rf $OPTDIR