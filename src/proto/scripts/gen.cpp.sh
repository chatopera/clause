#! /bin/bash 
###########################################
# NOTE: this script is retired.
# protobuf files are generated with CMakeLists.txt
###########################################

# constants
baseDir=$(cd `dirname "$0"`;pwd)
rootDir=$(cd $baseDir/../../..;pwd)
cppGenDir=$baseDir/../src/cpp
protoDir=$baseDir/../config
export PATH=$PATH:$rootDir/bin

# functions

# main 
[ -z "${BASH_SOURCE[0]}" -o "${BASH_SOURCE[0]}" = "$0" ] || return
cd $protoDir

if [ ! -d $cppGenDir ]; then
    mkdir -p $cppGenDir
fi

rm -rf $cppGenDir/*.h
rm -rf $cppGenDir/*.cc

for x in `ls *.proto`; do 
    echo "Generating source files for " $x "..."
    protoc -I$protoDir --cpp_out=$cppGenDir $x
done
