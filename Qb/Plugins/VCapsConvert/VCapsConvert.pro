# Webcamoid, webcam capture application.
# Copyright (C) 2011-2015  Gonzalo Exequiel Pedone
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
# Email   : hipersayan DOT x AT gmail DOT com
# Web-Site: http://github.com/hipersayanX/webcamoid

exists(commons.pri) {
    include(commons.pri)
} else {
    exists(../../commons.pri) {
        include(../../commons.pri)
    } else {
        error("commons.pri file not found.")
    }
}

CONFIG += plugin

DEFINES += __STDC_CONSTANT_MACROS

HEADERS += \
    include/vcapsconvert.h \
    include/vcapsconvertelement.h \
    include/convertio.h

INCLUDEPATH += \
    include \
    ../../include

!win32: LIBS += -L../../ -lQb
win32: LIBS += -L../../ -lQb$${VER_MAJ}

!isEmpty(FFMPEGINCLUDES): INCLUDEPATH += $${FFMPEGINCLUDES}
!isEmpty(FFMPEGLIBS): LIBS += $${FFMPEGLIBS}

OTHER_FILES += pspec.json

QT += qml

SOURCES += \
    src/vcapsconvert.cpp \
    src/vcapsconvertelement.cpp \
    src/convertio.cpp

DESTDIR = $${PWD}

TEMPLATE = lib

isEmpty(FFMPEGLIBS) {
    CONFIG += link_pkgconfig

    PKGCONFIG += \
        libavformat \
        libavcodec \
        libavutil \
        libswscale
}

INSTALLS += target

unix: target.path = $${LIBDIR}/$${COMMONS_TARGET}
!unix: target.path = $${PREFIX}/Qb/Plugins
