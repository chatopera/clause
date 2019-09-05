#! /bin/bash 
###########################################
# Install doxygen to generate docs
###########################################

# constants
baseDir=$(cd `dirname "$0"`;pwd)
ROOR_INSTALL_DIR=$baseDir/../..
SUDO_CMD=""

# functions

# main 
[ -z "${BASH_SOURCE[0]}" -o "${BASH_SOURCE[0]}" = "$0" ] || return
echo "Install pthread ..."

which sudo
if [ $? -eq 0 ]; then
    SUDO_CMD=sudo
fi

$SUDO_CMD apt-get install -y doxygen graphviz