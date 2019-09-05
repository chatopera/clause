#! /bin/bash 
###########################################
# CRFsuite - Tutorial on Chunking Task
# http://www.chokkan.org/software/crfsuite/tutorial.html
###########################################

# constants
baseDir=$(cd `dirname "$0"`;pwd)
rootDir=$(cd $baseDir/../..;pwd)

# functions

# main 
[ -z "${BASH_SOURCE[0]}" -o "${BASH_SOURCE[0]}" = "$0" ] || return

if [ ! -f $rootDir/bin/crfsuite ]; then
    echo "run ["$rootDir/cmake/user/002.crfsuite.sh"] first to install crfsuite."
    exit 1
fi

export PATH=$PATH:$rootDir/bin

cd $rootDir/build/crfsuite/example
wget http://www.clips.uantwerpen.be/conll2000/chunking/train.txt.gz
wget http://www.clips.uantwerpen.be/conll2000/chunking/test.txt.gz

zcat train.txt.gz | ./chunking.py > train.crfsuite.txt
zcat test.txt.gz | ./chunking.py > test.crfsuite.txt

crfsuite learn -m CoNLL2000.model train.crfsuite.txt
crfsuite tag -qt -m CoNLL2000.model test.crfsuite.txt

crfsuite dump CoNLL2000.model