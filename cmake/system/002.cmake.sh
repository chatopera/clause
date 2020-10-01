#! /bin/bash 
###########################################
#
# https://stackoverflow.com/questions/49859457/how-to-reinstall-the-latest-cmake-version
###########################################

# constants
baseDir=$(cd `dirname "$0"`;pwd)
ROOR_INSTALL_DIR=$baseDir/../..
SUDO_CMD=""

# functions

# main 
[ -z "${BASH_SOURCE[0]}" -o "${BASH_SOURCE[0]}" = "$0" ] || return
echo "Install latest cmake ..."

which sudo
if [ $? -eq 0 ]; then
    SUDO_CMD=sudo
fi

$SUDO_CMD apt-get purge cmake
$SUDO_CMD apt-get update
$SUDO_CMD apt-get install apt-transport-https ca-certificates gnupg \
                         software-properties-common wget

wget -qO - https://apt.kitware.com/keys/kitware-archive-latest.asc |
    $SUDO_CMD apt-key add -

$SUDO_CMD apt-add-repository 'deb https://apt.kitware.com/ubuntu/ xenial main'
$SUDO_CMD apt-get update
$SUDO_CMD apt-get install cmake
cmake -version

if [ $? -eq 0 ]; then
    echo "success."
    exit 0
else
    echo "fail."
    exit 1
fi