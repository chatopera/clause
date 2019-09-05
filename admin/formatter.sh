#! /bin/bash 
###########################################
# Format Code with Astyle
# http://astyle.sourceforge.net/astyle.html
###########################################

# constants
baseDir=$(cd `dirname "$0"`;pwd)
# functions

# main 
[ -z "${BASH_SOURCE[0]}" -o "${BASH_SOURCE[0]}" = "$0" ] || return
cd $baseDir/../src
astyle --project=../.astylerc \
    --ignore-exclude-errors \
     --exclude=/serving --recursive *.cpp,*.h,*.cc