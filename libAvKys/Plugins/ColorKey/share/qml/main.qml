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

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.platform as LABS
import Ak
import AkControls as AK
import ColorKeyElement

GridLayout {
    id: glyColorKey
    columns: 3

    readonly property string filePrefix: Ak.platform() == "windows"?
                                             "file:///":
                                             "file://"

    function toQrc(uri)
    {
        if (uri.indexOf(":") == 0)
            return "qrc" + uri

        return "file:" + uri
    }

    function optionIndex(cbx, option)
    {
        var index = -1

        for (var i = 0; i < cbx.model.count; i++)
            if (cbx.model.get(i).option == option) {
                index = i
                break
            }

        return index
    }

    Connections {
        target: ColorKey

        function onColorDiffChanged(colorDiff)
        {
            sldColorDiff.value = colorDiff
            spbColorDiff.value = colorDiff
        }

        function onSmoothnessChanged(smoothness)
        {
            sldSmoothness.value = smoothness
            spbSmoothness.value = smoothness
        }
    }

    Label {
        id: txtColor
        text: qsTr("Color")
    }
    RowLayout {
        Layout.columnSpan: 2

        Item {
            Layout.fillWidth: true
        }
        AK.ColorButton {
            currentColor: AkUtils.fromRgba(ColorKey.colorKey)
            title: qsTr("Choose the color to filter")
            modality: Qt.NonModal
            Accessible.description: txtColor.text

            onCurrentColorChanged: ColorKey.colorKey = AkUtils.toRgba(currentColor)
        }
    }

    Label {
        id: lblColorDiff
        text: qsTr("Color Difference")
    }
    Slider {
        id: sldColorDiff
        value: ColorKey.colorDiff
        stepSize: 1
        to: 512
        Layout.fillWidth: true
        Accessible.name: lblColorDiff.text

        onValueChanged: ColorKey.colorDiff = value
    }
    SpinBox {
        id: spbColorDiff
        value: ColorKey.colorDiff
        to: sldColorDiff.to
        stepSize: sldColorDiff.stepSize
        editable: true
        Accessible.name: lblColorDiff.text

        onValueChanged: ColorKey.colorDiff = Number(value)
    }

    Label {
        id: lblSmoothness
        text: qsTr("Smoothness")
    }
    Slider {
        id: sldSmoothness
        value: ColorKey.smoothness
        stepSize: 1
        to: 128
        Layout.fillWidth: true
        Accessible.name: lblSmoothness.text

        onValueChanged: ColorKey.smoothness = value
    }
    SpinBox {
        id: spbSmoothness
        value: ColorKey.smoothness
        to: sldSmoothness.to
        stepSize: sldSmoothness.stepSize
        editable: true
        Accessible.name: lblSmoothness.text

        onValueChanged: ColorKey.smoothness = Number(value)
    }

    Label {
        id: txtNormalize
        text: qsTr("Normalize")
    }
    RowLayout {
        Layout.columnSpan: 2

        Item {
            Layout.fillWidth: true
        }
        Switch {
            checked: ColorKey.normalize
            Accessible.name: txtNormalize.text

            onCheckedChanged: ColorKey.normalize = checked
        }
    }

    Label {
        id: txtMode
        text: qsTr("Background type")
    }
    ComboBox {
        id: cbxBackgroundType
        textRole: "text"
        currentIndex: optionIndex(cbxBackgroundType, ColorKey.backgroundType)
        Layout.fillWidth: true
        Layout.columnSpan: 2
        Accessible.description: txtMode.text
        model: ListModel {
            ListElement {
                text: qsTr("No background")
                option: ColorKeyElement.BackgroundTypeNoBackground
            }
            ListElement {
                text: qsTr("Color")
                option: ColorKeyElement.BackgroundTypeColor
            }
            ListElement {
                text: qsTr("Image")
                option: ColorKeyElement.BackgroundTypeImage
            }
        }

        onCurrentIndexChanged: ColorKey.backgroundType = cbxBackgroundType.model.get(currentIndex).option
    }

    Label {
        id: txtBackgroundColor
        text: qsTr("Background color")
        visible: cbxBackgroundType.currentIndex == 1
    }
    RowLayout {
        Layout.columnSpan: 2
        visible: cbxBackgroundType.currentIndex == 1

        Item {
            Layout.fillWidth: true
        }
        AK.ColorButton {
            currentColor: AkUtils.fromRgba(ColorKey.backgroundColor)
            title: qsTr("Choose the background color")
            showAlphaChannel: true
            Accessible.description: txtBackgroundColor.text

            onCurrentColorChanged: ColorKey.backgroundColor = AkUtils.toRgba(currentColor)
        }
    }

    RowLayout {
        Layout.columnSpan: 3
        visible: cbxBackgroundType.currentIndex == 2

        Image {
            width: 16
            height: 16
            fillMode: Image.PreserveAspectFit
            sourceSize.width: 16
            sourceSize.height: 16
            source: toQrc(txtTable.text)
        }
        TextField {
            id: txtTable
            text: ColorKey.background
            placeholderText: qsTr("Source palette")
            selectByMouse: true
            Layout.fillWidth: true
            Accessible.name: qsTr("Image file to use as palette")

            onTextChanged: ColorKey.background = text
        }
        Button {
            text: qsTr("Search")
            icon.source: "image://icons/search"
            Accessible.description: qsTr("Search the image file to use as palette")

            onClicked: fileDialog.open()
        }
    }

    LABS.FileDialog {
        id: fileDialog
        title: qsTr("Please choose an image file")
        nameFilters: ["Image files (*.bmp *.gif *.jpg *.jpeg *.png *.pbm *.pgm *.ppm *.xbm *.xpm)"]
        folder: glyColorKey.filePrefix + picturesPath

        onAccepted: ColorKey.background =
                    String(file).replace(glyColorKey.filePrefix, "")
    }
}
