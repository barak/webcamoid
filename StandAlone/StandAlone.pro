# Webcamoid, webcam capture application.
# Copyright (C) 2011-2016  Gonzalo Exequiel Pedone
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
    exists(../commons.pri) {
        include(../commons.pri)
    } else {
        error("commons.pri file not found.")
    }
}

!isEmpty(USE_GSTREAMER): DEFINES += USE_GSTREAMER

!isEmpty(BUILDDOCS):!isEqual(BUILDDOCS, 0) {
    DOCSOURCES += ../$${COMMONS_APPNAME}.qdocconf

    builddocs.input = DOCSOURCES
    builddocs.output = share/docs_auto/html/$${COMMONS_TARGET}.index
    builddocs.commands = $${QDOCTOOL} ${QMAKE_FILE_IN}
    builddocs.variable_out = DOCSOUTPUT
    builddocs.name = Docs ${QMAKE_FILE_IN}
    builddocs.CONFIG += target_predeps

    QMAKE_EXTRA_COMPILERS += builddocs
    PRE_TARGETDEPS += compiler_builddocs_make_all
}

CONFIG += qt

HEADERS = \
    src/mediatools.h \
    src/videodisplay.h \
    src/videoframe.h

INCLUDEPATH += \
    ../libAvKys/Lib/src

!win32: LIBS += -L../libAvKys/Lib -lAvKys
win32: LIBS += -L../libAvKys/Lib -lAvKys$${VER_MAJ}

OTHER_FILES = \
    share/effects.xml

QT += qml quick opengl widgets svg

RESOURCES += \
    Webcamoid.qrc \
    qml.qrc \
    translations.qrc \
    share/icons/icons.qrc

SOURCES = \
    src/main.cpp \
    src/mediatools.cpp \
    src/videodisplay.cpp \
    src/videoframe.cpp

lupdate_only {
    SOURCES = $$files(share/qml/*.qml)
}

TRANSLATIONS = $$files(share/ts/*.ts)

DESTDIR = $${PWD}

TARGET = $${COMMONS_TARGET}

!unix: RC_ICONS = share/icons/hicolor/256x256/webcamoid.ico

TEMPLATE = app

# http://www.loc.gov/standards/iso639-2/php/code_list.php

CODECFORTR = UTF-8
CODECFORSRC = UTF-8

INSTALLS += target
!unix: INSTALLS += \
    dllDeps \
    pluginsImageFormats \
    pluginsPlatform \
    pluginsQml

unix:target.path = $${BINDIR}
!unix:target.path = $${PREFIX}

!unix {
    DLLFILES = \
        \ # Qt
        $$[QT_INSTALL_BINS]/Qt5Core.dll \
        $$[QT_INSTALL_BINS]/Qt5Gui.dll \
        $$[QT_INSTALL_BINS]/Qt5Network.dll \
        $$[QT_INSTALL_BINS]/Qt5OpenGL.dll \
        $$[QT_INSTALL_BINS]/Qt5Qml.dll \
        $$[QT_INSTALL_BINS]/Qt5Quick.dll \
        $$[QT_INSTALL_BINS]/Qt5Svg.dll \
        $$[QT_INSTALL_BINS]/Qt5Widgets.dll \
        \ # System
        $$[QT_INSTALL_BINS]/libEGL.dll \
        $$[QT_INSTALL_BINS]/libGLESv2.dll \
        $$[QT_INSTALL_BINS]/libbz2-*.dll \
        $$[QT_INSTALL_BINS]/libfreetype-*.dll \
        $$[QT_INSTALL_BINS]/libgcc_s_seh-*.dll \
        $$[QT_INSTALL_BINS]/libgcc_s_sjlj-*.dll \
        $$[QT_INSTALL_BINS]/libglib-*.dll \
        $$[QT_INSTALL_BINS]/libharfbuzz-?.dll \
        $$[QT_INSTALL_BINS]/libiconv-*.dll \
        $$[QT_INSTALL_BINS]/libintl-*.dll \
        $$[QT_INSTALL_BINS]/libjpeg-*.dll \
        $$[QT_INSTALL_BINS]/libpcre-*.dll \
        $$[QT_INSTALL_BINS]/libpcre16-*.dll \
        $$[QT_INSTALL_BINS]/libpng16-*.dll \
        $$[QT_INSTALL_BINS]/libstdc++-*.dll \
        $$[QT_INSTALL_BINS]/libwinpthread-*.dll \
        $$[QT_INSTALL_BINS]/zlib1.dll

    isEmpty(FFMPEGLIBS) {
        DLLFILES += \
            \ # FFmpeg
            $$[QT_INSTALL_BINS]/avcodec-*.dll \
            $$[QT_INSTALL_BINS]/avdevice-*.dll \
            $$[QT_INSTALL_BINS]/avfilter-*.dll \
            $$[QT_INSTALL_BINS]/avformat-*.dll \
            $$[QT_INSTALL_BINS]/avresample-*.dll \
            $$[QT_INSTALL_BINS]/avutil-*.dll \
            $$[QT_INSTALL_BINS]/postproc-*.dll \
            $$[QT_INSTALL_BINS]/swresample-*.dll \
            $$[QT_INSTALL_BINS]/swscale-*.dll \
            \ # SDL
            $$[QT_INSTALL_BINS]/SDL.dll \
            \ # Codecs
            $$[QT_INSTALL_BINS]/libass-*.dll \
            $$[QT_INSTALL_BINS]/libbluray-*.dll \
            $$[QT_INSTALL_BINS]/libdcadec.dll \
            $$[QT_INSTALL_BINS]/libeay32.dll \
            $$[QT_INSTALL_BINS]/libexpat-*.dll \
            $$[QT_INSTALL_BINS]/libffi-*.dll \
            $$[QT_INSTALL_BINS]/libfontconfig-*.dll \
            $$[QT_INSTALL_BINS]/libfribidi-*.dll \
            $$[QT_INSTALL_BINS]/libgmp-*.dll \
            $$[QT_INSTALL_BINS]/libgnutls-*.dll \
            $$[QT_INSTALL_BINS]/libgomp-?.dll \
            $$[QT_INSTALL_BINS]/libgsm.dll.*.*.* \
            $$[QT_INSTALL_BINS]/libhogweed-*.dll \
            $$[QT_INSTALL_BINS]/libidn-*.dll \
            $$[QT_INSTALL_BINS]/liblzma-*.dll \
            $$[QT_INSTALL_BINS]/libmodplug-*.dll \
            $$[QT_INSTALL_BINS]/libmp3lame-*.dll \
            $$[QT_INSTALL_BINS]/libnettle-*.dll \
            $$[QT_INSTALL_BINS]/libogg-*.dll \
            $$[QT_INSTALL_BINS]/libopencore-amrnb-*.dll \
            $$[QT_INSTALL_BINS]/libopencore-amrwb-*.dll \
            $$[QT_INSTALL_BINS]/libopenjpeg-*.dll \
            $$[QT_INSTALL_BINS]/libopus-*.dll \
            $$[QT_INSTALL_BINS]/liborc-?.*.dll \
            $$[QT_INSTALL_BINS]/libp11-kit-*.dll \
            $$[QT_INSTALL_BINS]/libschroedinger-*.dll \
            $$[QT_INSTALL_BINS]/libsoxr.dll \
            $$[QT_INSTALL_BINS]/libspeex-*.dll \
            $$[QT_INSTALL_BINS]/libssh.dll \
            $$[QT_INSTALL_BINS]/libtasn1-*.dll \
            $$[QT_INSTALL_BINS]/libtheoradec-*.dll \
            $$[QT_INSTALL_BINS]/libtheoraenc-*.dll \
            $$[QT_INSTALL_BINS]/libvidstab.dll \
            $$[QT_INSTALL_BINS]/libvorbis-*.dll \
            $$[QT_INSTALL_BINS]/libvorbisenc-*.dll \
            $$[QT_INSTALL_BINS]/libvpx.dll.*.*.* \
            $$[QT_INSTALL_BINS]/libwebp-*.dll \
            $$[QT_INSTALL_BINS]/libwebpmux-*.dll \
            $$[QT_INSTALL_BINS]/libx264-*.dll \
            $$[QT_INSTALL_BINS]/libx265.dll \
            $$[QT_INSTALL_BINS]/libxml2-*.dll \
            $$[QT_INSTALL_BINS]/xvidcore.dll
    }

    dllDeps.files = $${DLLFILES}
    dllDeps.path = $${PREFIX}

    pluginsPlatform.files = $$[QT_INSTALL_PLUGINS]/platforms/qwindows.dll
    pluginsPlatform.path = $${PREFIX}/platforms

    pluginsImageFormats.files = $$[QT_INSTALL_PLUGINS]/imageformats/*
    pluginsImageFormats.path = $${PREFIX}/imageformats

    pluginsQml.files = $$[QT_INSTALL_QML]/*
    pluginsQml.path = $${PREFIX}/qml
}

!isEmpty(BUILDDOCS):!isEqual(BUILDDOCS, 0) {
    INSTALLS += docs

    docs.files = share/docs_auto/html
    docs.path = $${HTMLDIR}
    docs.CONFIG += no_check_exist
}
