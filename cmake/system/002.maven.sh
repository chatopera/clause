#! /bin/bash 
###########################################
#
###########################################

# constants
export JAVA_HOME=/usr/lib/jvm/java-1.8.0-amazon-corretto
export PATH=$PATH:$JAVA_HOME/bin
baseDir=$(cd `dirname "$0"`;pwd)
MVNNAME=maven.tgz
SUDO_CMD=""

# functions

# main 
[ -z "${BASH_SOURCE[0]}" -o "${BASH_SOURCE[0]}" = "$0" ] || return
which sudo
if [ $? -eq 0 ]; then
    SUDO_CMD=sudo
fi
cd /opt && $SUDO_CMD wget -O $MVNNAME https://downloads.apache.org/maven/maven-3/3.6.3/binaries/apache-maven-3.6.3-bin.tar.gz
$SUDO_CMD tar xzf $MVNNAME
$SUDO_CMD mv apache-maven-* maven
$SUDO_CMD rm $MVNNAME

set -x
export MAVEN_HOME=/opt/maven
export PATH=$PATH:$MAVEN_HOME/bin
mvn -version
