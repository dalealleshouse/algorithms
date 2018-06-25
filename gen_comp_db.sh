#!/bin/bash

###############################################################################
#
# Generates a compile_commands.json file used for intellisense by many editors
# Requires bear(http://manpages.ubuntu.com/manpages/artful/man1/bear.1.html)
# This must be run from the directory containing the c code.
# 
# 1) navigate to the c code directory
#       cd <algorithm>/c/
# 2) run the script
#       ../../gen_comp_db.sh
#
###############################################################################

mkdir src
# copy the Makefile used by the docker container
cp ../../docker/c/Makefile ./

# change the build directory
sed -i '' -e 's#/src/##g' Makefile

# generate the file with bear
bear make -B

# remove the Makefile
rm Makefile
rm -rf src
