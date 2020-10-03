#! /bin/bash 
#####################################################
# Relase new packages to dist branch with osc files
#####################################################

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

## Check git branch and status
GIT_BRANCH=`git branch | awk '/^\*/{print $2}'`

if [[ ! $GIT_BRANCH -eq 'osc' ]]; then
    echo "current branch is not osc."
    echo "switch to osc branch at first and release again."
    exit 1
fi

if [[ ! -z $(git status -s) ]]; then
    git status
    echo 'modified/untracked, fix them to keep clean and release again.'
    exit 2
fi

## Check if release dir exists.
relasedir=$baseDir/../../clause.dist
if [ ! -d $relasedir ]; then
    echo "ERROR" $relasedir "not exist"
    echo "Use following scripts to setup release dir at first."
    echo "  cd" `cd $baseDir/../..;pwd`
    echo "  git clone https://github.com/chatopera/clause.git -b dist clause.dist"
    exit 1
fi
relasedir=`cd $relasedir; pwd`

## Create temp dir
cd $baseDir/..
GIT_COMMIT_SHORT=`git rev-parse --short HEAD`
tmpdir=$baseDir/../tmp/$GIT_COMMIT_SHORT

if [ -d $tmpdir ]; then
    rm -rf $tmpdir
fi
echo "generate dir for temporary files $tmpdir ..."
mkdir -p $tmpdir

# Copy release files
## copy config
cd $baseDir/..
cp {sample.env,docker-compose.yml,LICENSE,README.md} $tmpdir

## copy database persistence folders
mkdir $tmpdir/var
cp -rf var/activemq $tmpdir/var
$SUDO_CMD rm -rf $tmpdir/var/activemq/data/*

cp -rf var/redis $tmpdir/var
$SUDO_CMD rm -rf $tmpdir/var/redis/data/*

cp -rf var/mysql $tmpdir/var
$SUDO_CMD rm -rf $tmpdir/var/mysql/data/*

## copy clause data folder
mkdir $tmpdir/var/local
cp -rf var/local/data $tmpdir/var/local/data
mkdir -p $tmpdir/var/local/workarea

## copy clause mysql database init script
mkdir -p $tmpdir/src/clause/config
cp -rf src/clause/config/sql $tmpdir/src/clause/config

## copy admin scripts
mkdir $tmpdir/admin
cp -rf admin/{flush.sh,start.sh,stop.sh} $tmpdir/admin

## generate BUILD file to save commit id
echo $GIT_COMMIT_SHORT > $tmpdir/BUILD
echo "" >> $tmpdir/sample.env
echo "CLAUSE_BUILD="$GIT_COMMIT_SHORT >> $tmpdir/sample.env

# Override release dir
cd $relasedir
rm -rf *
cd $tmpdir
tar cf - .|(cd $relasedir;tar xf -)
cd $relasedir
git status

echo $relasedir "is re-generated with" $GIT_COMMIT_SHORT "files."