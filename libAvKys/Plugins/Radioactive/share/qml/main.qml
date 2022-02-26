/* Webcamoid, webcam capture application.
 * Copyright (C) 2016  Gonzalo Exequiel Pedone
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
import QtQuick.Layouts 1.3
import Ak 1.0
import AkControls 1.0 as AK

GridLayout {
    columns: 2

    function modeIndex(mode)
    {
        var index = -1

        for (var i = 0; i < cbxMode.model.count; i++)
            if (cbxMode.model.get(i).mode == mode) {
                index = i
                break
            }

        return index
    }

    Label {
        text: qsTr("Mode")
    }
    ComboBox {
        id: cbxMode
        textRole: "text"
        currentIndex: modeIndex(Radioactive.mode)
        Layout.fillWidth: true

        model: ListModel {
            ListElement {
                text: qsTr("Soft normal")
                mode: "softNormal"
            }
            ListElement {
                text: qsTr("Hard normal")
                mode: "hardNormal"
            }
            ListElement {
                text: qsTr("Soft color")
                mode: "softColor"
            }
            ListElement {
                text: qsTr("Hard color")
                mode: "hardColor"
            }
        }

        onCurrentIndexChanged: Radioactive.mode = cbxMode.model.get(currentIndex).mode
    }

    Label {
        text: qsTr("Blur")
    }
    TextField {
        text: Radioactive.blur
        placeholderText: qsTr("Blur")
        selectByMouse: true
        validator: RegExpValidator {
            regExp: /\d+/
        }
        Layout.fillWidth: true

        onTextChanged: Radioactive.blur = Number(text)
    }
    Label {
        text: qsTr("Zoom")
    }
    TextField {
        text: Radioactive.zoom
        placeholderText: qsTr("Zoom")
        selectByMouse: true
        validator: RegExpValidator {
            regExp: /\d+\.\d+|\d+\.|\.\d+|\d+/
        }
        Layout.fillWidth: true

        onTextChanged: Radioactive.zoom = Number(text)
    }
    Label {
        text: qsTr("Threshold")
    }
    TextField {
        text: Radioactive.threshold
        placeholderText: qsTr("Threshold")
        selectByMouse: true
        validator: RegExpValidator {
            regExp: /\d+/
        }
        Layout.fillWidth: true

        onTextChanged: Radioactive.threshold = Number(text)
    }
    Label {
        id: lumaLabel
        /*: Minimum luminance/light/white level/intensity in a gray or black and
            white picture.

            https://en.wikipedia.org/wiki/Luma_(video)
         */
        text: qsTr("Luma threshold")
    }
    TextField {
        text: Radioactive.lumaThreshold
        placeholderText: lumaLabel.text
        selectByMouse: true
        validator: RegExpValidator {
            regExp: /\d+/
        }
        Layout.fillWidth: true

        onTextChanged: Radioactive.lumaThreshold = Number(text)
    }
    Label {
        id: alphaDiffLabel
        /*: Alpha channel, also known as the transparency component of a pixel
            in an image.
         */
        text: qsTr("Alpha differential")
    }
    TextField {
        text: Radioactive.alphaDiff
        placeholderText: alphaDiffLabel.text
        selectByMouse: true
        validator: RegExpValidator {
            regExp: /-?\d+/
        }
        Layout.fillWidth: true

        onTextChanged: Radioactive.alphaDiff = Number(text)
    }

    Label {
        text: qsTr("Radiation color")
    }
    RowLayout {
        Item {
            Layout.fillWidth: true
        }
        AK.ColorButton {
            currentColor: AkUtils.fromRgba(Radioactive.radColor)
            title: qsTr("Choose a color")
            showAlphaChannel: true

            onCurrentColorChanged: Radioactive.radColor = AkUtils.toRgba(currentColor)
        }
    }
}
