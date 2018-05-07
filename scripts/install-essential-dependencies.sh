#!/bin/bash

# Essential Dependencies
echo "Installing essential dependencies..."
apt-get -y update
apt-get -y install \
    git \
    wget \
    unzip \
    build-essential \
    llvm \
    clang-3.8 \
    libc++-dev \
    libc++abi-dev \
    cmake \
    libopenblas-dev \
    liblapack-dev \
    libgtk2.0-dev \
    pkg-config \
    libavcodec-dev \
    libavformat-dev \
    libswscale-dev \
    python-dev \
    python-numpy \
    libtbb2 \
    libtbb-dev \
    libjpeg-dev \
    libpng-dev \
    libtiff-dev \
    libdc1394-22-dev \
    checkinstall
echo "Done."

# Boost C++ Dependency
echo "Installing Boost..."
apt-get -y install libboost-all-dev
echo "Done."

# install FFMPEG
echo "Installing FFMPEG..."
apt-get -y install ffmpeg
echo "Done."
