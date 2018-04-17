#!/bin/bash

# Exit script if any command fails
set -e 
set -o pipefail

if [ $# -ne 0 ]
  then
    echo "Usage: install.sh"
    exit 1
fi

# OpenCV Dependencies!
cd OpenFace

echo "Downloading OpenCV..."
wget https://github.com/opencv/opencv/archive/3.4.0.zip
unzip 3.4.0.zip
cd opencv-3.4.0
mkdir -p build
cd build
echo "Installing OpenCV..."
cmake -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=/usr/local -D FORCE_VTK=ON -D WITH_TBB=ON -D WITH_V4L=ON -D WITH_OPENGL=ON -D WITH_GDAL=ON -D WITH_XINE=ON -D WITH_CUDA=OFF ..
make -j4
make install
cd ../..
rm 3.4.0.zip
rm -r opencv-3.4.0
echo "OpenCV installed."

# Boost C++ Dependency
echo "Installing Boost..."
apt-get -y install libboost-all-dev
echo "Boost installed."

# Dlib dependency
# OpenCV is supposed to download this for us, but I coudln't get their install to work
echo "Installing dlib..."
wget http://dlib.net/files/dlib-19.6.tar.bz2
tar xvf dlib-19.6.tar.bz2
cd dlib-19.6/
mkdir build
cd build
cmake ..
cmake --build . --config Release
make install
ldconfig
cd ../..
echo "dlib installed."

# OpenFace installation
echo "Installing OpenFace..."
mkdir -p build
cd build
cmake -D CMAKE_BUILD_TYPE=RELEASE ..
make install
cd ../..
echo "OpenFace successfully installed."
