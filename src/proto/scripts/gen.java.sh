#! /bin/bash 
###########################################
# NOTE: this script is retired.
# protobuf files are generated with CMakeLists.txt
###########################################

# constants
baseDir=$(cd `dirname "$0"`;pwd)
rootDir=$(cd $baseDir/../../..;pwd)
javaGenDir=$baseDir/../src/java
protoDir=$baseDir/../config
export PATH=$PATH:$rootDir/bin

# functions

# main 
[ -z "${BASH_SOURCE[0]}" -o "${BASH_SOURCE[0]}" = "$0" ] || return
cd $protoDir

if [ ! -d $javaGenDir ]; then
    mkdir -p $javaGenDir
fi

rm -rf $javaGenDir/*.java

for x in `ls *.proto`; do 
    echo "Generating source files for " $x "..."
    protoc -I=$protoDir --java_out=$javaGenDir $x
done
