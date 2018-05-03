#!/bin/bash
# OpenCV Dependencies!
echo "Build + Install OpenCV..."
mkdir -p $DEPENDENCIES_BUILD_DIR/opencv-3.4.0 && cd $DEPENDENCIES_BUILD_DIR/opencv-3.4.0
cmake -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=/usr/local -D FORCE_VTK=ON -D WITH_TBB=ON -D WITH_V4L=ON -D WITH_OPENGL=ON -D WITH_GDAL=ON -D WITH_XINE=ON -D WITH_CUDA=OFF $DEPENDENCIES_DIR/opencv-3.4.0
make -j4
make install
echo "Done."
