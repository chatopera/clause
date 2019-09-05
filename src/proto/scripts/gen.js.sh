#! /bin/bash 
###########################################
# NOTE: this script is retired.
# protobuf files are generated with CMakeLists.txt
###########################################

# constants
baseDir=$(cd `dirname "$0"`;pwd)
rootDir=$(cd $baseDir/../../..;pwd)
jsGenDir=$baseDir/../src/js
protoDir=$baseDir/../config
export PATH=$PATH:$rootDir/bin

# functions

# main 
[ -z "${BASH_SOURCE[0]}" -o "${BASH_SOURCE[0]}" = "$0" ] || return
cd $protoDir

if [ ! -d $jsGenDir ]; then
    mkdir -p $jsGenDir
fi

rm -rf $jsGenDir/*.js

for x in `ls *.proto`; do 
    echo "Generating source files for " $x "..."
    protoc -I$protoDir --js_out=import_style=commonjs,binary:$jsGenDir $x
done
