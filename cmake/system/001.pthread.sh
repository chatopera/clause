#! /bin/bash 
###########################################
# Install pthread as a lib
###########################################

# constants
baseDir=$(cd `dirname "$0"`;pwd)
ROOR_INSTALL_DIR=$baseDir/../..
THIRD_PARTY=$ROOR_INSTALL_DIR/thirdparty
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

set -x
$SUDO_CMD apt-get update
$SUDO_CMD apt-get install -y libpthread-stubs0-dev
