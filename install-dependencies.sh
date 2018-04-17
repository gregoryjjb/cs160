#!/bin/bash

# Essential Dependencies
echo "Installing Essential dependencies..."
apt-get -y update
apt-get -y install build-essential
apt-get -y install llvm
apt-get -y install clang-3.8 libc++-dev libc++abi-dev
apt-get -y install cmake
apt-get -y install libopenblas-dev liblapack-dev
apt-get -y install libgtk2.0-dev pkg-config libavcodec-dev libavformat-dev libswscale-dev
apt-get -y install python-dev python-numpy libtbb2 libtbb-dev libjpeg-dev libpng-dev libtiff-dev libdc1394-22-dev checkinstall
apt-get -y install git wget unzip
echo "Essential dependencies installed."

# install FFMPEG
echo "Installing FFMPEG..."
apt-get -y install ffmpeg
echo "FFMPEG installed."
