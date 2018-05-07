#!/bin/bash

# Exit script if any command fails
set -e
set -o pipefail

if [ $# -ne 0 ]
  then
    echo "Usage: install.sh"
    exit 1
fi


echo "Extracting dependencies from archives..."
unzip opencv-3.4.0.zip
tar xvf dlib-19.6.tar.bz2
mkdir -p OpenFace
tar xvf OpenFace_v1.0.0.tar.gz -C OpenFace --strip-components=1
echo "Done."

BASE_DIR="$(pwd)"
BUILD_DIR="${BASE_DIR}/build"
mkdir -p $BUILD_DIR
