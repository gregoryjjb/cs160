#!/bin/bash
# OpenFace installation
echo "Build + Install OpenFace..."
mkdir -p $DEPENDENCIES_BUILD_DIR/OpenFace && cd $DEPENDENCIES_BUILD_DIR/OpenFace
cmake -D CMAKE_BUILD_TYPE=RELEASE $DEPENDENCIES_DIR/OpenFace
make install
echo "Done."
