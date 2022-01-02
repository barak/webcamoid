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

if [ -z "${DISABLE_CCACHE}" ]; then
    if [ "${CXX}" = clang++ ]; then
        UNUSEDARGS="-Qunused-arguments"
    fi

    COMPILER="ccache ${CXX} ${UNUSEDARGS}"
else
    COMPILER=${CXX}
fi

if [ "${ARCH_ROOT_BUILD}" = 1 ]; then
    EXEC='sudo ./root.x86_64/bin/arch-chroot root.x86_64'
elif [ "${TRAVIS_OS_NAME}" = linux ] && [ -z "${ANDROID_BUILD}" ]; then
    if [ -z "${DAILY_BUILD}" ]; then
        EXEC="docker exec ${DOCKERSYS}"
    else
        EXEC="docker exec -e DAILY_BUILD=1 ${DOCKERSYS}"
    fi
fi

BUILDSCRIPT=dockerbuild.sh

if [ "${DOCKERIMG}" = ubuntu:bionic ]; then
    cat << EOF > ${BUILDSCRIPT}
#!/bin/sh

source /opt/qt${PPAQTVER}/bin/qt${PPAQTVER}-env.sh
EOF

    chmod +x ${BUILDSCRIPT}
fi

if [ "${ANDROID_BUILD}" = 1 ]; then
    export JAVA_HOME=$(readlink -f /usr/bin/java | sed 's:bin/java::')
    export ANDROID_HOME="${PWD}/build/android-sdk"
    export ANDROID_NDK="${PWD}/build/android-ndk"
    export ANDROID_NDK_HOME=${ANDROID_NDK}
    export ANDROID_NDK_PLATFORM=android-${ANDROID_PLATFORM}
    export ANDROID_NDK_ROOT=${ANDROID_NDK}
    export ANDROID_SDK_ROOT=${ANDROID_HOME}
    export PATH="${JAVA_HOME}/bin/java:${PATH}"
    export PATH="$PATH:${ANDROID_HOME}/tools:${ANDROID_HOME}/tools/bin"
    export PATH="${PATH}:${ANDROID_HOME}/platform-tools"
    export PATH="${PATH}:${ANDROID_HOME}/emulator"
    export PATH="${PATH}:${ANDROID_NDK}"
    export ORIG_PATH="${PATH}"

    for arch_ in $(echo "${TARGET_ARCH}" | tr ":" "\n"); do
        export PATH="${PWD}/build/Qt/${QTVER_ANDROID}/android/bin:${ORIG_PATH}"
        mkdir build-webcamoid-${arch_}
        cd build-webcamoid-${arch_}
        qmake -query
        qmake -spec ${COMPILESPEC} ../Webcamoid.pro \
            CONFIG+=silent \
            ANDROID_ABIS=${arch_}
        cd ..
    done
elif [ "${ARCH_ROOT_BUILD}" = 1 ]; then
    sudo mount --bind root.x86_64 root.x86_64
    sudo mount --bind $HOME root.x86_64/$HOME

    if [ -z "${ARCH_ROOT_MINGW}" ]; then
        QMAKE_CMD=qmake
    else
        QMAKE_CMD=/usr/${ARCH_ROOT_MINGW}-w64-mingw32/lib/qt/bin/qmake
    fi

    cat << EOF > ${BUILDSCRIPT}
#!/bin/sh

export LC_ALL=C
export HOME=$HOME
EOF

    if [ ! -z "${DAILY_BUILD}" ]; then
        cat << EOF >> ${BUILDSCRIPT}
export DAILY_BUILD=1
EOF
    fi

    cat << EOF >> ${BUILDSCRIPT}
cd $TRAVIS_BUILD_DIR
${QMAKE_CMD} -query
${QMAKE_CMD} -spec ${COMPILESPEC} Webcamoid.pro \
    CONFIG+=silent \
    QMAKE_CXX="${COMPILER}"
EOF
    chmod +x ${BUILDSCRIPT}
    sudo cp -vf ${BUILDSCRIPT} root.x86_64/$HOME/

    ${EXEC} bash $HOME/${BUILDSCRIPT}
elif [ "${TRAVIS_OS_NAME}" = linux ]; then
    export PATH=$HOME/.local/bin:$PATH

    if [ "${DOCKERSYS}" = debian ]; then
        if [ "${DOCKERIMG}" = ubuntu:bionic ]; then
            if [ -z "${DAILY_BUILD}" ] && [ -z "${RELEASE_BUILD}" ]; then
                cat << EOF >> ${BUILDSCRIPT}
#!/bin/sh

qmake -query
qmake -spec ${COMPILESPEC} Webcamoid.pro \
    CONFIG+=silent \
    QMAKE_CXX="${COMPILER}"
EOF
            else
                cat << EOF >> ${BUILDSCRIPT}
#!/bin/sh

qmake -query
qmake -spec ${COMPILESPEC} Webcamoid.pro \
    CONFIG+=silent \
    QMAKE_CXX="${COMPILER}" \
    NOGSTREAMER=1 \
    NOLIBAVDEVICE=1 \
    NOQTAUDIO=1
EOF
            fi

            ${EXEC} bash ${BUILDSCRIPT}
        else
            ${EXEC} qmake -qt=5 -query
            ${EXEC} qmake -qt=5 -spec ${COMPILESPEC} Webcamoid.pro \
                CONFIG+=silent \
                QMAKE_CXX="${COMPILER}"
        fi
    else
        ${EXEC} qmake-qt5 -query
        ${EXEC} qmake-qt5 -spec ${COMPILESPEC} Webcamoid.pro \
            CONFIG+=silent \
            QMAKE_CXX="${COMPILER}"
    fi
elif [ "${TRAVIS_OS_NAME}" = osx ]; then
    ${EXEC} qmake -query

    if [ -z "${DAILY_BUILD}" ] && [ -z "${RELEASE_BUILD}" ]; then
        ${EXEC} qmake -spec ${COMPILESPEC} Webcamoid.pro \
            CONFIG+=silent \
            QMAKE_CXX="${COMPILER}"
    else
        ${EXEC} qmake -spec ${COMPILESPEC} Webcamoid.pro \
            CONFIG+=silent \
            QMAKE_CXX="${COMPILER}" \
            NOGSTREAMER=1 \
            NOJACK=1 \
            NOLIBUVC=1 \
            NOPULSEAUDIO=1 \
            NOQTAUDIO=1
    fi
fi

if [ -z "${NJOBS}" ]; then
    NJOBS=4
fi

if [ "${ANDROID_BUILD}" = 1 ]; then
    for arch_ in $(echo "${TARGET_ARCH}" | tr ":" "\n"); do
        export PATH="${PWD}/build/Qt/${QTVER_ANDROID}/android/bin:${ORIG_PATH}"
        cd build-webcamoid-${arch_}
        make -j${NJOBS}
        cd ..
    done
elif [ "${ARCH_ROOT_BUILD}" = 1 ]; then
    cat << EOF > ${BUILDSCRIPT}
#!/bin/sh

export LC_ALL=C
export HOME=$HOME
EOF

    if [ ! -z "${DAILY_BUILD}" ]; then
        cat << EOF >> ${BUILDSCRIPT}
export DAILY_BUILD=1
EOF
    fi

    cat << EOF >> ${BUILDSCRIPT}
cd $TRAVIS_BUILD_DIR
make -j${NJOBS}
EOF
    chmod +x ${BUILDSCRIPT}
    sudo cp -vf ${BUILDSCRIPT} root.x86_64/$HOME/

    ${EXEC} bash $HOME/${BUILDSCRIPT}
    sudo umount root.x86_64/$HOME
    sudo umount root.x86_64
else
    ${EXEC} make -j${NJOBS}
fi
