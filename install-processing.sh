#!/bin/bash

# Install our processing application
echo "Installing processing application..."
#cd cs160/CVProcessor
make CONF=Release
cd ../..
rm -rf OpenFace
#cd cs160
echo "Processing application installed. Located in cs160/CVProcessor/dist/Release"
