#! /bin/bash 
###########################################################
# Boost C++ libraries setup
# Author: Hai Liang Wang <hain@chatopera.com>
###########################################################

# constants
baseDir=$(cd `dirname "$0"`;pwd)
ROOR_INSTALL_DIR=/usr/local
# Constant values definitions
FOLDER_NAME=boost
OPTDIR=/opt/$FOLDER_NAME
BUILD=build-Release
BOOST_VERSION=1_70_0
BOOST_VERSION_PATH_SUFFIX=1.70.0
SUDO_CMD=""

# main 
[ -z "${BASH_SOURCE[0]}" -o "${BASH_SOURCE[0]}" = "$0" ] || return
which sudo
if [ $? -eq 0 ]; then
    SUDO_CMD=sudo
fi

# Starting setup of Boost
echo "Setting up Boost" $BOOST_VERSION "..."
 
#----------------------------------------------------------
# Installing dependent packages
#----------------------------------------------------------
 
# Inform the user about the next action
echo "Installing the dependent packages build-essential g++ gcc libicu-dev..."

# remove old files
$SUDO_CMD dpkg -P libboost-all-dev libboost-dev libboost-test-dev
$SUDO_CMD rm -rf $ROOR_INSTALL_DIR/lib/libboost_*
$SUDO_CMD rm -rf $ROOR_INSTALL_DIR/include/boost

# Execute the action
$SUDO_CMD apt-get update && $SUDO_CMD apt-get -y install \
    build-essential g++ gcc mpi-default-dev libicu-dev python-dev \
    python3-dev libbz2-dev zlib1g-dev \
    libicu-dev zip unzip wget

#----------------------------------------------------------
# Installing Boost
#----------------------------------------------------------
 
# Inform the user about the next action
echo "Downloading and installing Boost ..."
 
# Create a new folder for storing the source code
if [ -d $OPTDIR ]; then
    $SUDO_CMD rm -rf $OPTDIR
fi

$SUDO_CMD mkdir -p $OPTDIR;
set -x
cd $OPTDIR
 
# Download source code
$SUDO_CMD wget http://sourceforge.net/projects/boost/files/boost/$BOOST_VERSION_PATH_SUFFIX/boost_$BOOST_VERSION.zip
 
# Extract archive
$SUDO_CMD unzip -qq boost_$BOOST_VERSION.zip
$SUDO_CMD chmod 777 -R boost_$BOOST_VERSION
# Change directory
cd boost_$BOOST_VERSION
 
# Run the script which prepares Boost's build process
$SUDO_CMD ./bootstrap.sh \
    --prefix=$ROOR_INSTALL_DIR \
    --with-libraries=all
 
# Compile the project
nprocs=`cat /proc/cpuinfo| grep "processor"| wc -l`
$SUDO_CMD ./b2 define=_GLIBCXX_USE_CXX11_ABI=0 \
    cxxflags="-fPIC" \
    -j${nprocs} install

# Add the Boost libraries path to the default Ubuntu library search path
$SUDO_CMD sh -c 'echo "/usr/local/lib" > /etc/ld.so.conf.d/boost.conf'

# Update the default Ubuntu library search paths
$SUDO_CMD ldconfig

# clean up files
cd /opt; $SUDO_CMD rm -rf $OPTDIR
 
echo "Boost was successfully installed."
