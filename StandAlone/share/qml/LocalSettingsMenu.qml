/* Webcamoid, webcam capture application.
 * Copyright (C) 2023  Gonzalo Exequiel Pedone
 *
 * Webcamoid is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Webcamoid is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Webcamoid. If not, see <http://www.gnu.org/licenses/>.
 *
 * Web-Site: http://webcamoid.github.io/
 */

import QtQuick 2.12
import QtQuick.Controls 2.5
import Ak 1.0
import Webcamoid 1.0

Menu {
    id: settingsMenu
    margins: AkUnit.create(16 * AkTheme.controlScale, "dp").pixels

    signal copyToClipboard()
    signal openImageCaptureSettings()

    MenuItem {
        text: qsTr("Copy to clipboard")
        icon.source: "image://icons/paperclip"

        onClicked: settingsMenu.copyToClipboard()
    }
    MenuItem {
        text: qsTr("Image capture settings")
        icon.source: "image://icons/settings"
        visible: videoLayer.deviceType(videoLayer.videoInput) == VideoLayer.InputCamera

        onClicked: settingsMenu.openImageCaptureSettings()
    }
}
