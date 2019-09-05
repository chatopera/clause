#! /bin/bash 
###########################################
#
###########################################

# constants
baseDir=$(cd `dirname "$0"`;pwd)
#NODEPKG=https://nodejs.org/dist/v8.16.0/node-v8.16.0-linux-x64.tar.xz
NODEPKG=https://nodejs.org/dist/v10.16.0/node-v10.16.0-linux-x64.tar.xz
SUDO_CMD=""
INSTALLDIR=/opt/node

# functions

# main 
[ -z "${BASH_SOURCE[0]}" -o "${BASH_SOURCE[0]}" = "$0" ] || return
which sudo
if [ $? -eq 0 ]; then
    SUDO_CMD=sudo
fi

if [ -d $INSTALLDIR ]; then
    echo "node dir exist. /opt/node"
else
    cd /opt; $SUDO_CMD wget -O node.tar.xz $NODEPKG
    $SUDO_CMD tar xf node.tar.xz; $SUDO_CMD rm node.tar.xz
    $SUDO_CMD mv node-* node
    set -x 
    export NODE_HOME=/opt/node
    export PATH=$PATH:$NODE_HOME/bin
    $SUDO_CMD ln -s /opt/node/bin/node /usr/bin/node
    $SUDO_CMD ln -s /opt/node/bin/npm /usr/bin/npm
    node -v
fi
