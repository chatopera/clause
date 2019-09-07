#! /bin/bash 
###########################################
# 
###########################################

# constants
baseDir=$(cd `dirname "$0"`;pwd)
rootDir=$(cd $baseDir/..;pwd)
SUDO_CMD=""
# functions

# main 
[ -z "${BASH_SOURCE[0]}" -o "${BASH_SOURCE[0]}" = "$0" ] || return
which sudo
if [ $? -eq 0 ]; then
    SUDO_CMD=sudo
fi

if [ ! -d $baseDir/../tmp ]; then
    mkdir $baseDir/../tmp
fi

cd $baseDir/..
GIT_COMMIT_SHORT=`git rev-parse --short HEAD`
relasedir=$baseDir/../tmp/clause.dist.$GIT_COMMIT_SHORT

cd $baseDir/../tmp
rm -rf $relasedir; mkdir $relasedir

# copy config file
cp $rootDir/sample.env $relasedir/.env
cp $rootDir/docker-compose.yml $relasedir
cp $rootDir/LICENSE $relasedir
cp $rootDir/README.md $relasedir

# copy database persistence folders
mkdir $relasedir/var
cp -rf $rootDir/var/activemq $relasedir/var
$SUDO_CMD rm -rf $relasedir/var/activemq/data/*

cp -rf $rootDir/var/redis $relasedir/var
$SUDO_CMD rm -rf $relasedir/var/redis/data/*

cp -rf $rootDir/var/mysql $relasedir/var
$SUDO_CMD rm -rf $relasedir/var/mysql/data/*

# copy clause data folder
mkdir $relasedir/var/local
cp -rf $rootDir/var/local/data $relasedir/var/local/data
mkdir -p $relasedir/var/local/workarea

# copy clause mysql database init script
mkdir -p $relasedir/src/clause/config
cp -rf $rootDir/src/clause/config/sql $relasedir/src/clause/config

# copy admin scripts
mkdir $relasedir/admin
cp -rf $rootDir/admin/docker $relasedir/admin

# init git
cp -rf $rootDir/.gitignore $relasedir
cd $relasedir
git init 
git add --all
git commit -m "init"
cd ..

tar czf clause.dist.$GIT_COMMIT_SHORT.tgz clause.dist.$GIT_COMMIT_SHORT