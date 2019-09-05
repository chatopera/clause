#! /bin/bash 
###########################################
# Install jsoncpp as a lib
# https://github.com/open-source-parsers/jsoncpp/wiki/Building
###########################################

# constants
baseDir=$(cd `dirname "$0"`;pwd)
ROOR_INSTALL_DIR=$baseDir/../..
THIRD_PARTY=$ROOR_INSTALL_DIR/thirdparty
SRCDIR=$ROOR_INSTALL_DIR/thirdparty/jsoncpp
OPTDIR=$ROOR_INSTALL_DIR/build/jsoncpp
SUDO_CMD=""

# functions

# main 
[ -z "${BASH_SOURCE[0]}" -o "${BASH_SOURCE[0]}" = "$0" ] || return
echo "Install jsoncpp ..."

which sudo
if [ $? -eq 0 ]; then
    SUDO_CMD=sudo
fi

##################################
# Due to Alibaba ASR's deps
# have to use jsoncpp dynamic lib
##################################
# echo "remove old packages ..."
# rm -rf $OPTDIR; mkdir $OPTDIR
# cd $OPTDIR
# cmake -DCMAKE_INSTALL_PREFIX=$ROOR_INSTALL_DIR \
#     -G "Unix Makefiles" $SRCDIR
# make 

##################################
# Due to Alibaba ASR's deps
# have to use jsoncpp dynamic lib
##################################
echo "Install jsoncpp as dynamic lib ..."
$SUDO_CMD cp -rf $SRCDIR/lib/* $ROOR_INSTALL_DIR/lib
$SUDO_CMD rm -rf $ROOR_INSTALL_DIR/include/json
$SUDO_CMD cp -rf $SRCDIR/include/* $ROOR_INSTALL_DIR/include

##################################
# build json lib from source code
# put source code in thirdparty/jsoncpp
# https://github.com/open-source-parsers/jsoncpp/
##################################
# echo "remove old packages ..."
# rm -rf $OPTDIR; mkdir $OPTDIR
# cd $OPTDIR
# cmake -DCMAKE_BUILD_TYPE=release \
#      -DBUILD_STATIC_LIBS=ON \
#      -DBUILD_SHARED_LIBS=ON \
#      -DCMAKE_CXX_STANDARD=11 \
#      -DCMAKE_INSTALL_PREFIX=$ROOR_INSTALL_DIR -G "Unix Makefiles" $THIRD_PARTY/jsoncpp

# make
# $SUDO_CMD make install
# $SUDO_CMD rm -rf $OPTDIR
