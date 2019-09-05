#! /bin/bash 
###########################################
# Install default gcc 5.4.0
###########################################

# constants
baseDir=$(cd `dirname "$0"`;pwd)
SUDO_CMD=""

# functions

# main 
[ -z "${BASH_SOURCE[0]}" -o "${BASH_SOURCE[0]}" = "$0" ] || return
which sudo
if [ $? -eq 0 ]; then
    SUDO_CMD=sudo
fi

$SUDO_CMD apt-get update && $SUDO_CMD apt-get install uuid-dev
