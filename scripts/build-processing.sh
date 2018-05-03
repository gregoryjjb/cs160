#!/bin/bash

# Build our processing application
echo "Building processing application..."
cd $PROJECT_DIR/CVProcessor
make CONF=Release
echo "Done."
