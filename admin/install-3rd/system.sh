#! /bin/bash 
###########################################
#
###########################################

# constants
baseDir=$(cd `dirname "$0"`;pwd)
THIRD_PARTY=$(cd $baseDir/../../thirdparty;pwd)
ROOR_INSTALL_DIR=$(cd $baseDir/../..;pwd)
SUDO_CMD=""

# functions

# main 
[ -z "${BASH_SOURCE[0]}" -o "${BASH_SOURCE[0]}" = "$0" ] || return
which sudo
if [ $? -eq 0 ]; then
    SUDO_CMD=sudo
fi

echo "Install 3rd party assets ..."
cd $ROOR_INSTALL_DIR/cmake/system

for x in `find . -name "*.sh"|sort`; do
    echo "[install] run" $x " ..."
    $x
    if [ ! $? -eq 0 ]; then
        echo "Failed result with" $x 
        exit 1
    fi
done

