#!/bin/bash
echo "please run it in the building-docker\n"
pushd ~/workspace
./build-scripts/build.sh -c br prepare
./build-scripts/build.sh -c br build
./build-scripts/build.sh -c br package
popd