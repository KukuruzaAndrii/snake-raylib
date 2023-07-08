# only for Ubuntu

set -ex
sudo apt install libasound2-dev libx11-dev libxrandr-dev libxi-dev libgl1-mesa-dev libglu1-mesa-dev libxcursor-dev libxinerama-dev
wget https://github.com/raysan5/raylib/releases/download/4.5.0/raylib-4.5.0_linux_amd64.tar.gz
tar -xzf raylib-4.5.0_linux_amd64.tar.gz
sudo cp raylib-4.5.0_linux_amd64/include/* /usr/local/include/
sudo cp raylib-4.5.0_linux_amd64/lib/*.so* /usr/local/lib/
sudo ldconfig
rm -rf raylib-4.5.0_linux_amd64/
rm -f raylib-4.5.0_linux_amd64.tar.gz

