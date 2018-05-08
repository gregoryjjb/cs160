#!/bin/bash

# Download + extract dependencies that we will need to build
echo "Downloading OpenCV..."
wget https://github.com/opencv/opencv/archive/3.4.0.zip opencv-3.4.0.zip
echo "Done."

echo "Downloading dlib..."
wget http://dlib.net/files/dlib-19.6.tar.bz2
echo "Done."

echo "Downloading OpenFace..."
wget https://github.com/TadasBaltrusaitis/OpenFace/archive/OpenFace_v1.0.0.tar.gz
echo "Done."
