#!/bin/bash

# Webcamoid, webcam capture application.
# Copyright (C) 2017  Gonzalo Exequiel Pedone
#
# Webcamoid is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Webcamoid is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Webcamoid. If not, see <http://www.gnu.org/licenses/>.
#
# Web-Site: http://webcamoid.github.io/

if [ "${COMPILER}" = clang ]; then
    COMPILER_C=clang
    COMPILER_CXX=clang++
else
    COMPILER_C=gcc
    COMPILER_CXX=g++
fi

if [ -z "${DISABLE_CCACHE}" ]; then
    EXTRA_PARAMS="-DCMAKE_C_COMPILER_LAUNCHER=ccache -DCMAKE_CXX_COMPILER_LAUNCHER=ccache -DCMAKE_OBJCXX_COMPILER_LAUNCHER=ccache"
fi

if [ "${UPLOAD}" == 1 ]; then
    EXTRA_PARAMS="${EXTRA_PARAMS} -DNOGSTREAMER=ON -DNOLIBAVDEVICE=ON"
fi

EXEC="docker exec ${DOCKERSYS}"
BUILDSCRIPT=dockerbuild.sh

if [ "${DOCKERIMG}" = ubuntu:focal ]; then
    cat << EOF > ${BUILDSCRIPT}
#!/bin/sh
source /opt/qt${PPAQTVER}/bin/qt${PPAQTVER}-env.sh
EOF

    chmod +x ${BUILDSCRIPT}
fi

export PATH=$HOME/.local/bin:$PATH

if [ "${DOCKERIMG}" = ubuntu:focal ]; then
    if [ "${UPLOAD}" != 1 ]; then
        cat << EOF >> ${BUILDSCRIPT}
#!/bin/sh

mkdir build
cmake \
    -S . \
    -B build \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX="\${PWD}/webcamoid-data" \
    -DCMAKE_C_COMPILER="${COMPILER_C}" \
    -DCMAKE_CXX_COMPILER="${COMPILER_CXX}" \
    ${EXTRA_PARAMS} \
    -DDAILY_BUILD=${DAILY_BUILD}
cmake -LA -S . -B build
cmake --build build --parallel ${NJOBS}
cmake --install build
EOF
    else
        cat << EOF >> ${BUILDSCRIPT}
#!/bin/sh

mkdir build
cmake \
    -S . \
    -B build \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX="\${PWD}/webcamoid-data" \
    -DCMAKE_C_COMPILER="${COMPILER_C}" \
    -DCMAKE_CXX_COMPILER="${COMPILER_CXX}" \
    ${EXTRA_PARAMS} \
    -DDAILY_BUILD=${DAILY_BUILD} \
    -DNOGSTREAMER=TRUE \
    -DNOLIBAVDEVICE=TRUE
cmake -LA -S . -B build
cmake --build build --parallel ${NJOBS}
cmake --install build
EOF
    fi

        ${EXEC} bash ${BUILDSCRIPT}
else
    cat << EOF >> ${BUILDSCRIPT}
#!/bin/sh

mkdir build
cmake \
    -S . \
    -B build \
    -DQT_QMAKE_EXECUTABLE="qmake -qt=5" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX="\${PWD}/webcamoid-data" \
    -DCMAKE_C_COMPILER="${COMPILER_C}" \
    -DCMAKE_CXX_COMPILER="${COMPILER_CXX}" \
    ${EXTRA_PARAMS} \
    -DDAILY_BUILD=${DAILY_BUILD}
cmake -LA -S . -B build
cmake --build build --parallel ${NJOBS}
cmake --install build
EOF
fi

chmod +x ${BUILDSCRIPT}
${EXEC} bash ${BUILDSCRIPT}