#! /bin/bash 
###########################################
# Install pthread as a lib
###########################################

# constants
baseDir=$(cd `dirname "$0"`;pwd)
ROOR_INSTALL_DIR=$baseDir/../..
THIRD_PARTY=$ROOR_INSTALL_DIR/thirdparty
OPTDIR=/admin
SUDO_CMD=""

# functions

# main 
[ -z "${BASH_SOURCE[0]}" -o "${BASH_SOURCE[0]}" = "$0" ] || return
echo "Install daemontools ..."

which sudo
if [ $? -eq 0 ]; then
    SUDO_CMD=sudo
fi

if [ ! -d $OPTDIR ]; then
    $SUDO_CMD mkdir -p $OPTDIR
fi

set -x
cd $THIRD_PARTY/daemontools/admin
tar cf - .|(cd $OPTDIR; $SUDO_CMD tar xf -)
cd $OPTDIR/daemontools-0.76

$SUDO_CMD package/install

if [ $? -eq 0 ]; then
    echo "Install daemontools successfully."
else
    exit 1
fi

ls -al /command

$SUDO_CMD cp $THIRD_PARTY/daemontools/svscan.conf /etc/init/svscan.conf
$SUDO_CMD chown root:root /etc/init/svscan.conf
$SUDO_CMD chmod 644 /etc/init/svscan.conf