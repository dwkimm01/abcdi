#!/usr/bin/env bash

export CC=/usr/bin/clang-18
export CXX=/usr/bin/clang++-18

pipx ensurepath
export PATH=~/.local/bin:${PATH}
export PATH=${HOME}/opt/cmake-3.30.5/bin:${PATH}

# Note, this should go into docker build
rm -v ~/.conan2/profiles/default
conan profile detect
sed -i 's/gnu17/gnu23/g' /root/.conan2/profiles/default && \

echo "Finding cmake at"
which cmake

export BUILDDIR="./cmake-build-debug"
rm -Rfv ${BUILDDIR}
mkdir ${BUILDDIR}

conan install . -of ${BUILDDIR} --build=missing
cmake --preset conan-release -B ${BUILDDIR} -DCMAKE_TOOLCHAIN_FILE:FILEPATH=${BUILDDIR}/build/Release/generators/conan_toolchain.cmake
cmake --build ${BUILDDIR} -j 3
${BUILDDIR}/tests/unit/abcdi_test
