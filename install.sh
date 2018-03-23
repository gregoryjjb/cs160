# start from the terminal in a directory you are okay with adding files to
# will place two folders at this level: OpenFace and cs160

# Exit script if any command fails
set -e 
set -o pipefail

if [ $# -ne 0 ]
  then
    echo "Usage: install.sh"
    exit 1
fi

# install FFMPEG
sudo apt-get -y install ffmpeg

# Install OpenFace and all of it's dependencies
git clone https://github.com/TadasBaltrusaitis/OpenFace
cd OpenFace

# Essential Dependencies
echo "Installing Essential dependencies..."
sudo apt-get -y update
sudo apt-get -y install build-essential
sudo apt-get -y install llvm
sudo apt-get -y install clang-3.7 libc++-dev libc++abi-dev
sudo apt-get -y install cmake
sudo apt-get -y install libopenblas-dev liblapack-dev
sudo apt-get -y install git libgtk2.0-dev pkg-config libavcodec-dev libavformat-dev libswscale-dev
sudo apt-get -y install python-dev python-numpy libtbb2 libtbb-dev libjpeg-dev libpng-dev libtiff-dev libjasper-dev libdc1394-22-dev checkinstall
echo "Essential dependencies installed."

# OpenCV Dependency
echo "Downloading OpenCV..."
wget -O https://github.com/opencv/opencv/archive/3.4.0.zip
unzip 3.4.0.zip
cd opencv-3.4.0
mkdir -p build
cd build
echo "Installing OpenCV..."
cmake -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=/usr/local -D FORCE_VTK=ON -D WITH_TBB=ON -D WITH_V4L=ON -D WITH_OPENGL=ON -D WITH_GDAL=ON -D WITH_XINE=ON ..
make -j4
sudo make install
cd ../..
rm 3.4.0.zip
sudo rm -r opencv-3.4.0
echo "OpenCV installed."

# Boost C++ Dependency
echo "Installing Boost..."
sudo apt-get install libboost-all-dev
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
sudo make install
sudo ldconfig
cd ..
echo "dlib installed."

# OpenFace installation
echo "Installing OpenFace..."
mkdir -p build
cd build
cmake -D CMAKE_BUILD_TYPE=RELEASE ..
sudo make install
cd ../..
echo "OpenFace successfully installed."

# Install our processing application
echo "Installing processing application..."
git clone https://github.com/gregdumb/cs160
cd cs160/CVProcessor
make CONF=Release
cd ../..
echo "Processing application installed. Located in cs160/dist/Release"

