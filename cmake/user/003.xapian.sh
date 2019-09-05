#! /bin/bash 
###########################################
# Install xapian as a lib
# https://gitlab.chatopera.com/chatopera/xapian-rev
###########################################

# constants
baseDir=$(cd `dirname "$0"`;pwd)
ROOR_INSTALL_DIR=$baseDir/../..
THIRD_PARTY=$ROOR_INSTALL_DIR/thirdparty
OPTDIR=$ROOR_INSTALL_DIR/build/xapian
TARFILE=$ROOR_INSTALL_DIR/thirdparty/xapian/xapian-core.1.4.10.tgz
SUDO_CMD=""

# functions

# main 
[ -z "${BASH_SOURCE[0]}" -o "${BASH_SOURCE[0]}" = "$0" ] || return
echo "Install xapian libs ..."

which sudo
if [ $? -eq 0 ]; then
    SUDO_CMD=sudo
fi

if [ -f $TARFILE ]; then
    cd $ROOR_INSTALL_DIR
    if [ -d $OPTDIR ]; then
        $SUDO_CMD rm -rf $OPTDIR
    fi
    mkdir -p $OPTDIR; cd $OPTDIR
    $SUDO_CMD tar xzf $TARFILE; cd xapian-core.1.4.10
    $SUDO_CMD rm -rf lib/cmake
    
    if [ ! -z $SUDO_CMD ]; then
        $SUDO_CMD bash -c "tar cf - .|(cd $ROOR_INSTALL_DIR; tar xf -)"
    else
        tar cf - .|(cd $ROOR_INSTALL_DIR; tar xf -)
    fi
    echo "Done."
else
    echo $TARFILE "not exist."
    exit 1
fi


