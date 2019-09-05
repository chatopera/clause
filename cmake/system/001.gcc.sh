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

$SUDO_CMD apt-get install --reinstall -y \
    build-essential g++ gcc git astyle gdb \
    cmake autoconf automake libtool libtool-bin

set -x
if [ -f /usr/local/bin/gcc ]; then
    $SUDO_CMD mv /usr/local/bin/gcc /usr/local/bin/gcc.bak
fi

if [ -f /usr/local/bin/g++ ]; then
    $SUDO_CMD mv /usr/local/bin/g++ /usr/local/bin/g++.bak
fi

if [ -f /usr/local/bin/c++ ]; then
    $SUDO_CMD mv /usr/local/bin/c++ /usr/local/bin/c++.bak
fi

if [ -f /usr/local/bin/cpp ]; then
    $SUDO_CMD mv /usr/local/bin/cpp /usr/local/bin/cpp.bak
fi
