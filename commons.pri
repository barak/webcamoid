# Webcamoid, webcam capture application.
# Copyright (C) 2012  Gonzalo Exequiel Pedone
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

VER_MAJ = 8
VER_MIN = 5
VER_PAT = 0
VERSION = $${VER_MAJ}.$${VER_MIN}.$${VER_PAT}

isEmpty(BUILDDOCS): BUILDDOCS = 0
isEmpty(QDOCTOOL): {
    unix: QDOCTOOL = $$[QT_INSTALL_BINS]/qdoc
    !unix: QDOCTOOL = $$[QT_INSTALL_LIBEXECS]/qdoc
}
isEmpty(QMAKE_LRELEASE) {
    unix: QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease
    !unix: QMAKE_LRELEASE = $$[QT_INSTALL_LIBEXECS]/lrelease
}
isEmpty(QMAKE_LUPDATE) {
    unix: QMAKE_LUPDATE = $$[QT_INSTALL_BINS]/lupdate
    !unix: QMAKE_LUPDATE = $$[QT_INSTALL_LIBEXECS]/lupdate
}

isEmpty(BUNDLENAME): BUNDLENAME = webcamoid

win32 {
    host_name = $$lower($$QMAKE_HOST.os)

    !isEmpty(ProgramW6432) {
        DEFAULT_PREFIX = $(ProgramW6432)/$${BUNDLENAME}
    } else: !isEmpty(ProgramFiles) {
        DEFAULT_PREFIX = $(ProgramFiles)/$${BUNDLENAME}
    } else: contains(host_name, linux) {
        DEFAULT_PREFIX = /$${BUNDLENAME}
    } else {
        DEFAULT_PREFIX = C:/$${BUNDLENAME}
    }
} else: macx: isEmpty(NOAPPBUNDLE) {
    DEFAULT_PREFIX = /Applications
} else {
    DEFAULT_PREFIX = /usr
}

isEmpty(PREFIX): PREFIX = $${DEFAULT_PREFIX}
isEmpty(EXECPREFIX) {
    macx: isEmpty(NOAPPBUNDLE) {
        EXECPREFIX = $${PREFIX}/$${BUNDLENAME}.app/Contents
    } else {
        EXECPREFIX = $${PREFIX}
    }
}
isEmpty(BINDIR) {
    macx: isEmpty(NOAPPBUNDLE) {
        BINDIR = $${EXECPREFIX}/MacOS
    } else {
        BINDIR = $${EXECPREFIX}/bin
    }
}
isEmpty(SBINDIR): SBINDIR = $${EXECPREFIX}/sbin
isEmpty(LIBEXECDIR): LIBEXECDIR = $${EXECPREFIX}/libexec
isEmpty(DATAROOTDIR) {
    macx: isEmpty(NOAPPBUNDLE) {
        DATAROOTDIR = $${EXECPREFIX}/Resources
    } else {
        DATAROOTDIR = $${PREFIX}/share
    }
}
isEmpty(DATDIR): DATDIR = $${DATAROOTDIR}/$${COMMONS_TARGET}
isEmpty(SYSCONFDIR): SYSCONFDIR = $${PREFIX}/etc
isEmpty(SHAREDSTATEDIR): SHAREDSTATEDIR = $${PREFIX}/com
isEmpty(LOCALSTATEDIR): LOCALSTATEDIR = $${PREFIX}/var
isEmpty(INCLUDEDIR): INCLUDEDIR = $${PREFIX}/include
isEmpty(DOCDIR): DOCDIR = $${DATAROOTDIR}/doc/$${COMMONS_TARGET}
isEmpty(INFODIR): INFODIR = $${DATAROOTDIR}/info
isEmpty(HTMLDIR): HTMLDIR = $${DOCDIR}/html
isEmpty(DVIDIR): DVIDIR = $${DOCDIR}/dvi
isEmpty(PDFDIR): PDFDIR = $${DOCDIR}/pdf
isEmpty(PSDIR): PSDIR = $${DOCDIR}/ps
isEmpty(LIBDIR) {
    macx: isEmpty(NOAPPBUNDLE) {
        LIBDIR = $${EXECPREFIX}/Frameworks
    } else: win32 {
        LIBDIR = $${BINDIR}
    } else {
        LIBDIR = $${EXECPREFIX}/lib
    }
}
isEmpty(LOCALEDIR): LOCALEDIR = $${DATAROOTDIR}/locale
isEmpty(MANDIR): MANDIR = $${DATAROOTDIR}/man
isEmpty(LICENSEDIR): LICENSEDIR = $${DATAROOTDIR}/licenses/$${COMMONS_TARGET}
isEmpty(LOCALDIR): LOCALDIR = $${PREFIX}/local
isEmpty(LOCALLIBDIR): LOCALLIBDIR = $${LOCALDIR}/lib
isEmpty(INSTALLQMLDIR) {
    macx: isEmpty(NOAPPBUNDLE) {
        INSTALLQMLDIR = $${DATAROOTDIR}/qml
    } else: win32 {
        INSTALLQMLDIR = $${EXECPREFIX}/lib/qt/qml
    } else {
        INSTALLQMLDIR = $${LIBDIR}/qt/qml
    }
}
isEmpty(INSTALLPLUGINSDIR) {
    macx: isEmpty(NOAPPBUNDLE) {
        INSTALLPLUGINSDIR = $${EXECPREFIX}/Plugins/$${COMMONS_TARGET}
    } else: win32 {
        INSTALLPLUGINSDIR = $${EXECPREFIX}/lib/$${COMMONS_TARGET}
    } else {
        INSTALLPLUGINSDIR = $${LIBDIR}/$${COMMONS_TARGET}
    }
}

macx: !isEmpty(NOAPPBUNDLE): DEFINES += NOAPPBUNDLE

DEFINES += \
    COMMONS_APPNAME=\"\\\"$$COMMONS_APPNAME\\\"\" \
    COMMONS_TARGET=\"\\\"$$COMMONS_TARGET\\\"\" \
    COMMONS_VER_MAJ=\"\\\"$$VER_MAJ\\\"\" \
    COMMONS_VERSION=\"\\\"$$VERSION\\\"\" \
    PREFIX=\"\\\"$$PREFIX\\\"\" \
    EXECPREFIX=\"\\\"$$EXECPREFIX\\\"\" \
    BINDIR=\"\\\"$$BINDIR\\\"\" \
    SBINDIR=\"\\\"$$SBINDIR\\\"\" \
    LIBEXECDIR=\"\\\"LIBEXECDIR\\\"\" \
    DATAROOTDIR=\"\\\"$$DATAROOTDIR\\\"\" \
    DATDIR=\"\\\"$$DATDIR\\\"\" \
    SYSCONFDIR=\"\\\"$$SYSCONFDIR\\\"\" \
    SHAREDSTATEDIR=\"\\\"$$SHAREDSTATEDIR\\\"\" \
    LOCALSTATEDIR=\"\\\"$$LOCALSTATEDIR\\\"\" \
    INCLUDEDIR=\"\\\"$$INCLUDEDIR\\\"\" \
    DOCDIR=\"\\\"$$DOCDIR\\\"\" \
    INFODIR=\"\\\"$$INFODIR\\\"\" \
    HTMLDIR=\"\\\"$$HTMLDIR\\\"\" \
    DVIDIR=\"\\\"$$DVIDIR\\\"\" \
    PDFDIR=\"\\\"$$PDFDIR\\\"\" \
    PSDIR=\"\\\"$$PSDIR\\\"\" \
    LIBDIR=\"\\\"$$LIBDIR\\\"\" \
    LOCALEDIR=\"\\\"$$LOCALEDIR\\\"\" \
    MANDIR=\"\\\"$$MANDIR\\\"\" \
    LICENSEDIR=\"\\\"$$LICENSEDIR\\\"\" \
    LOCALDIR=\"\\\"$$LOCALDIR\\\"\" \
    LOCALLIBDIR=\"\\\"$$LOCALLIBDIR\\\"\" \
    INSTALLQMLDIR=\"\\\"$$INSTALLQMLDIR\\\"\" \
    INSTALLPLUGINSDIR=\"\\\"$$INSTALLPLUGINSDIR\\\"\"

TARGET_ARCH = $${QMAKE_TARGET.arch}

mingw {
    TARGET_ARCH = $$system($${QMAKE_CC} -dumpmachine)
    TARGET_ARCH = $$split(TARGET_ARCH, -)
    TARGET_ARCH = $$first(TARGET_ARCH)
}

msvc {
    TARGET_ARCH = $$basename(TARGET_ARCH)
    TARGET_ARCH = $$replace(TARGET_ARCH, x64, x86_64)
}

CONFIG(debug, debug|release) {
    COMMONS_BUILD_PATH = debug/Qt$${QT_VERSION}/$$basename(QMAKE_CC)/$${TARGET_ARCH}
    DEFINES += QT_DEBUG
} else {
    COMMONS_BUILD_PATH = release/Qt$${QT_VERSION}/$$basename(QMAKE_CC)/$${TARGET_ARCH}
}

BIN_DIR = $${COMMONS_BUILD_PATH}/bin
MOC_DIR = $${COMMONS_BUILD_PATH}/moc
OBJECTS_DIR = $${COMMONS_BUILD_PATH}/obj
RCC_DIR = $${COMMONS_BUILD_PATH}/rcc
UI_DIR = $${COMMONS_BUILD_PATH}/ui

# Update translations.
isEmpty(NOLUPDATE): !isEmpty(TRANSLATIONS_PRI): CONFIG(debug, debug|release) {
    updatetr.commands = $$QMAKE_LUPDATE -no-obsolete $$TRANSLATIONS_PRI
    QMAKE_EXTRA_TARGETS += updatetr
    PRE_TARGETDEPS += updatetr
}

# Compile translations files.
isEmpty(NOLRELEASE): !isEmpty(TRANSLATIONS): CONFIG(debug, debug|release) {
    compiletr.input = TRANSLATIONS
    compiletr.output = ${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.qm
    compiletr.commands = $$QMAKE_LRELEASE -removeidentical -compress ${QMAKE_FILE_IN} -qm ${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.qm
    compiletr.clean = dummy_file
    compiletr.CONFIG += no_link
    QMAKE_EXTRA_COMPILERS += compiletr
    PRE_TARGETDEPS += compiler_compiletr_make_all
}

win32 {
    CONFIG += skip_target_version_ext
    !isEmpty(STATIC_BUILD):!isEqual(STATIC_BUILD, 0) {
        win32-g++: QMAKE_LFLAGS = -static-libgcc -static-libstdc++
    }
}
macx: QT_CONFIG -= no-pkg-config

# Enable c++11 support in all platforms
!CONFIG(c++11): CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

lessThan(QT_MAJOR_VERSION, 5) | lessThan(QT_MINOR_VERSION, 9) {
    QT_VER_STR = $${QT_MAJOR_VERSION}.$${QT_MINOR_VERSION}.$${QT_PATCH_VERSION}
    error("Qt 5.9.0 or higher required, current installed version is $${QT_VER_STR}")
}

!qtHaveModule(quickcontrols2) {
    error("QtQuick Controls 2 required.")
}

CMD_SEP = $$escape_expand(\n\t)
