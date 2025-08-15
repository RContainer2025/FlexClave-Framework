#!/bin/bash
echo "please run it in the building-docker\n"
pushd ~/workspace
./build-scripts/build.sh -c kernel prepare
./build-scripts/build.sh -c kernel build
./build-scripts/build.sh -c kernel package
popd