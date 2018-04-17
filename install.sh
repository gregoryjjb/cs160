#!/bin/bash

# Get the script's parent directory
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# start from inside cs160 folder. 
# Will add a directory at the same level as the cs160 folder for OpenFace

cd ..

$DIR/install-dependencies.sh
$DIR/download-openface.sh
$DIR/install-openface.sh
$DIR/install-processing.sh
