#!/bin/sh

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

pkg update -f
pkg install -y \
    git \
    pkgconf \
    python3 \
    qt5-qmake \
    qt5-buildtools \
    qt5-concurrent \
    qt5-multimedia \
    qt5-quickcontrols \
    qt5-quickcontrols2 \
    qt5-svg \
    qt5-xml \
    ffmpeg \
    gstreamer1 \
    alsa-lib \
    jackit \
    pulseaudio \
    libuvc
