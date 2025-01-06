#!/usr/bin/env bash

pipx ensurepath
export PATH=~/.local/bin:${PATH}
export PATH=${HOME}/opt/cmake-3.30.5/bin:${PATH}

echo "Finding cmake at"
which cmake

export BUILDDIR="./cmake-build-debug"
rm -Rfv ${BUILDDIR}
mkdir ${BUILDDIR}

conan install . -of ${BUILDDIR} --build=missing
cmake --preset conan-release -B ${BUILDDIR} -DCMAKE_TOOLCHAIN_FILE:FILEPATH=${BUILDDIR}/build/Release/generators/conan_toolchain.cmake
cmake --build ${BUILDDIR} -j 3
${BUILDDIR}/tests/unit/abcdi_test
