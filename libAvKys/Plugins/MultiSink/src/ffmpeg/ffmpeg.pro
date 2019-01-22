# Webcamoid, webcam capture application.
# Copyright (C) 2016  Gonzalo Exequiel Pedone
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

exists(akcommons.pri) {
    include(akcommons.pri)
} else {
    exists(../../../../akcommons.pri) {
        include(../../../../akcommons.pri)
    } else {
        error("akcommons.pri file not found.")
    }
}

CONFIG += plugin

HEADERS = \
    src/plugin.h \
    src/mediawriterffmpeg.h \
    ../mediawriter.h \
    src/abstractstream.h \
    src/videostream.h \
    src/audiostream.h

INCLUDEPATH += \
    ../../../../Lib/src \
    ../

LIBS += -L$${OUT_PWD}/../../../../Lib/$${BIN_DIR} -l$${COMMONS_TARGET}

OTHER_FILES += pspec.json

DEFINES += __STDC_CONSTANT_MACROS

!isEmpty(FFMPEGINCLUDES): INCLUDEPATH += $${FFMPEGINCLUDES}
!isEmpty(FFMPEGLIBS): LIBS += $${FFMPEGLIBS}

isEmpty(FFMPEGLIBS) {
    CONFIG += link_pkgconfig

    PKGCONFIG += \
        libavformat \
        libavcodec \
        libswscale \
        libavutil
}

CONFIG(config_ffmpeg_avcodec_sendrecv): \
    DEFINES += HAVE_SENDRECV
CONFIG(config_ffmpeg_avformat_codecpar): \
    DEFINES += HAVE_CODECPAR
CONFIG(config_ffmpeg_avutil_extraoptions): \
    DEFINES += HAVE_EXTRAOPTIONS

QT += qml concurrent

SOURCES = \
    src/plugin.cpp \
    src/mediawriterffmpeg.cpp \
    ../mediawriter.cpp \
    src/abstractstream.cpp \
    src/videostream.cpp \
    src/audiostream.cpp

DESTDIR = $${OUT_PWD}/../../$${BIN_DIR}/submodules/MultiSink

TEMPLATE = lib

INSTALLS += target
target.path = $${INSTALLPLUGINSDIR}/submodules/MultiSink
