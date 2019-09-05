#! /bin/bash 
###########################################
# Install pthread as a lib
###########################################

# constants
baseDir=$(cd `dirname "$0"`;pwd)
ROOR_INSTALL_DIR=$baseDir/../..
THIRD_PARTY=$ROOR_INSTALL_DIR/thirdparty
OPTDIR=$ROOR_INSTALL_DIR/build/thrift
BUILD=build-Release
SUDO_CMD=""
export LD_LIBRARY_PATH=$ROOR_INSTALL_DIR/lib:$LD_LIBRARY_PATH
export JAVA_HOME=/usr/lib/jvm/java-1.8.0-amazon-corretto

# functions

# main 
[ -z "${BASH_SOURCE[0]}" -o "${BASH_SOURCE[0]}" = "$0" ] || return
echo "Install pthread ..."

# which thrift
# if [ $? -eq 0 ]; then
#     echo "Resolve thrift" `thrift --version`
#     exit 0
# fi

which sudo
if [ $? -eq 0 ]; then
    SUDO_CMD=sudo
fi

set -x
$SUDO_CMD apt-get update
$SUDO_CMD apt-get install -y \
    libtool \
    flex \
    bison \
    pkg-config \
    g++ \
    libssl-dev

echo "Remove old version files ..."
$SUDO_CMD rm -rf $ROOR_INSTALL_DIR/lib/libthrift*
$SUDO_CMD rm -rf $ROOR_INSTALL_DIR/bin/thrift
$SUDO_CMD rm -rf $ROOR_INSTALL_DIR/include/thrift

$SUDO_CMD rm -rf $OPTDIR
mkdir $OPTDIR && cd $OPTDIR
tar -xf $THIRD_PARTY/thrift/thrift-0.12.0.tar.gz

cd thrift-0.12.0
# ./configure \
CXXFLAGS="-D_GLIBCXX_USE_CXX11_ABI=0" ./configure \
        --prefix=$ROOR_INSTALL_DIR \
        --with-boost=/usr/local \
        --with-libevent=/usr \
        --with-boost-libdir=/usr/local/lib \
        --with-lua=no \
        --with-d=no \
        --with-c_glib=no \
        --with-ruby=no \
        --with-dart=no \
        --with-haskell=no \
        --with-rs=no \
        --with-cl=no \
        --with-php=no \
        --with-php_extension=no \
        --with-erlang=no \
        --without-tests
nprocs=`cat /proc/cpuinfo| grep "processor"| wc -l`
make -j${nprocs} && $SUDO_CMD make install
# $SUDO_CMD echo "export LD_LIBRARY_PATH=\$LD_LIBRARY_PATH:$ROOR_INSTALL_DIR/lib/" >> /etc/environment
# cd .. && $SUDO_CMD rm -rf thrift* 

cd $ROOR_INSTALL_DIR
$ROOR_INSTALL_DIR/bin/thrift -version
