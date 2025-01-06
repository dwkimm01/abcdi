#!/usr/bin/env bash

echo "System issue"
cat /etc/issue

export DEBIAN_FRONTEND=noninteractive
apt -y update
cat /etc/issue
uname -a
add-apt-repository universe
apt -y update
apt -y upgrade
apt -y install build-essential libssl-dev cmake pipx wget



add-apt-repository ppa:ubuntu-toolchain-r/test
apt -y update
apt -y install gcc-13 g++-13
## update-alternatives --set g++ /usr/bin/g++-13
## update-alternatives --set gcc /usr/bin/gcc-13

update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-11 10
update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-13 20

update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-11 10
update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-13 20

##update-alternatives --install /usr/bin/cc cc /usr/bin/gcc 30
##update-alternatives --set cc /usr/bin/gcc

##update-alternatives --install /usr/bin/c++ c++ /usr/bin/g++ 30
##update-alternatives --set c++ /usr/bin/g++

#update-alternatives: using /usr/bin/gcc-11 to provide /usr/bin/gcc (gcc) in auto mode
#update-alternatives: using /usr/bin/gcc-13 to provide /usr/bin/gcc (gcc) in auto mode
#update-alternatives: using /usr/bin/g++-11 to provide /usr/bin/g++ (g++) in auto mode
#update-alternatives: using /usr/bin/g++-13 to provide /usr/bin/g++ (g++) in auto mode


mkdir ~/Downloads
cd ~/Downloads
wget https://github.com/Kitware/CMake/releases/download/v3.30.5/cmake-3.30.5.tar.gz
tar -xvzf cmake-3.30.5.tar.gz
cd cmake-3.30.5
mkdir -vp ${HOME}/opt/cmake-3.30.5
./bootstrap --prefix=${HOME}/opt/cmake-3.30.5
make -j2
make install

export PATH=${HOME}/opt/cmake-3.30.5/bin:${PATH}

pipx install conan
pipx ensurepath
export PATH=~/.local/bin:${PATH}
conan profile detect
sed -i 's/gnu17/gnu23/g' ${HOME}/.conan2/profiles/default
echo "Resulting conan profile is"
cat ~/.conan2/profiles/default
