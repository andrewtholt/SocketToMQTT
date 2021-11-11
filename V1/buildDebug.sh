#!/bin/bash

# set -x

echo "=============="
echo "Building for Debug"
echo "=============="

rm -rf Debug
mkdir Debug

cd Debug

cmake -DCMAKE_BUILD_TYPE=Debug .. && make
