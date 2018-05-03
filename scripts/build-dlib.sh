#!/bin/bash
# Dlib dependency
# OpenCV is supposed to download this for us, but I coudln't get their install to work
echo "Build + Install dlib..."
mkdir -p $DEPENDENCIES_BUILD_DIR/dlib-19.6 && cd $DEPENDENCIES_BUILD_DIR/dlib-19.6
cmake $DEPENDENCIES_DIR/dlib-19.6
cmake --build $DEPENDENCIES_DIR/dlib-19.6 --config Release
make install
ldconfig
echo "Done."
