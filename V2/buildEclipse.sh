#!/bin/bash

# set -x

echo "=============="
echo "Building for Eclipse"
echo "=============="

HERE=$(pwd)

STUB=$(basename $HERE)

ECLIPSE=$(echo "../${STUB}_Eclipse")

if [ ! -d $ECLIPSE ]; then
    mkdir $ECLIPSE
fi

cd $ECLIPSE

cmake -G "Eclipse CDT4 - Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_ECLIPSE_GENERATE_SOURCE_PROJECT=TRUE -DCMAKE_ECLIPSE_MAKE_ARGUMENTS=-j8 ../${STUB}
exit 0
