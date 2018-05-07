#!/bin/bash

# Get the script's parent directory
PROJECT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
SCRIPTS_DIR="${PROJECT_DIR}/scripts"
DEPENDENCIES_DIR="${PROJECT_DIR}/dependencies"
DEPENDENCIES_BUILD_DIR="${DEPENDENCIES_DIR}/build"
CVPROCESSOR_DIR="{PROJECT_DIR}/CVProcessor"
PYTHON_SERVER_DIR="${PROJECT_DIR}/PythonServer"

mkdir -p $DEPENDENCIES_DIR

cd $DEPENDENCIES_DIR && $SCRIPTS_DIR/download-dependencies.sh
cd $DEPENDENCIES_DIR && $SCRIPTS_DIR/install-essential-dependencies.sh
cd $DEPENDENCIES_DIR && $SCRIPTS_DIR/setup-dependency-builds.sh

cd $DEPENDENCIES_BUILD_DIR
$SCRIPTS_DIR/build-opencv.sh
$SCRIPTS_DIR/build-dlib.sh
$SCRIPTS_DIR/build-openface.sh

cd $CVPROCESSOR_DIR && $SCRIPTS_DIR/build-processing.sh
cd $PYTHON_SERVER_DIR && $SCRIPTS_DIR/setup-python-server.sh
