#!/bin/bash

# set -x
echo "=============="
echo "Building for Release"
echo "=============="

rm -rf Release
mkdir Release

cd Release

cmake -DCMAKE_BUILD_TYPE=Release .. && make
