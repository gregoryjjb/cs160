#!/bin/bash

# Download OpenFace...it's big...

#echo "Git-pulling latest OpenFace..."
#git clone https://github.com/TadasBaltrusaitis/OpenFace
wget https://github.com/TadasBaltrusaitis/OpenFace/archive/OpenFace_v1.0.0.tar.gz
mkdir OpenFace
tar xvf OpenFace_v1.0.0.tar.gz -C OpenFace --strip-components=1
