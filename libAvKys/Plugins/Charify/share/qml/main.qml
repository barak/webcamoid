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
import Qt.labs.platform 1.1 as LABS
import QtQuick.Layouts 1.3
import Ak 1.0
import AkControls 1.0 as AK

GridLayout {
    columns: 2

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

    Label {
        text: qsTr("Mode")
    }
    ComboBox {
        id: cbxMode
        textRole: "text"
        currentIndex: optionIndex(cbxMode, Charify.mode)
        Layout.fillWidth: true

        model: ListModel {
            ListElement {
                text: qsTr("Natural")
                option: "natural"
            }
            ListElement {
                text: qsTr("Fixed")
                option: "fixed"
            }
        }

        onCurrentIndexChanged: Charify.mode = cbxMode.model.get(currentIndex).option
    }

    Label {
        text: qsTr("Symbols")
    }
    TextField {
        text: Charify.charTable
        placeholderText: qsTr("Symbols")
        selectByMouse: true
        Layout.fillWidth: true

        onTextChanged: Charify.charTable = text
    }

    Label {
        text: qsTr("Font")
    }
    RowLayout {
        TextField {
            id: txtTable
            text: Charify.font.family + " " + Charify.font.pointSize
            placeholderText: qsTr("Font")
            selectByMouse: true
            readOnly: true
            font: Charify.font
            Layout.fillWidth: true
        }
        Button {
            text: qsTr("Search")
            icon.source: "image://icons/fonts"

            onClicked: fontDialog.open()
        }
    }

    Label {
        text: qsTr("Hinting")
    }
    ComboBox {
        id: cbxHinting
        textRole: "text"
        currentIndex: optionIndex(cbxHinting, Charify.hintingPreference)
        Layout.fillWidth: true

        model: ListModel {
            ListElement {
                text: qsTr("Default")
                option: "PreferDefaultHinting"
            }
            ListElement {
                text: qsTr("No hinting")
                option: "PreferNoHinting"
            }
            ListElement {
                text: qsTr("Vertical hinting")
                option: "PreferVerticalHinting"
            }
            ListElement {
                text: qsTr("Full hinting")
                option: "PreferFullHinting"
            }
        }

        onCurrentIndexChanged: Charify.hintingPreference = cbxHinting.model.get(currentIndex).option
    }

    Label {
        //: Different font rendering strategies
        text: qsTr("Style")
    }
    ComboBox {
        id: cbxStyle
        textRole: "text"
        currentIndex: optionIndex(cbxStyle, Charify.styleStrategy)
        Layout.fillWidth: true

        model: ListModel {
            ListElement {
                text: qsTr("Default")
                option: "PreferDefault"
            }
            ListElement {
                text: qsTr("Bitmap")
                option: "PreferBitmap"
            }
            ListElement {
                text: qsTr("Device")
                option: "PreferDevice"
            }
            ListElement {
                text: qsTr("Outline")
                option: "PreferOutline"
            }
            ListElement {
                text: qsTr("Force outline")
                option: "ForceOutline"
            }
            ListElement {
                text: qsTr("Match")
                option: "PreferMatch"
            }
            ListElement {
                text: qsTr("Quality")
                option: "PreferQuality"
            }
            ListElement {
                text: qsTr("Antialias")
                option: "PreferAntialias"
            }
            ListElement {
                text: qsTr("No antialias")
                option: "NoAntialias"
            }
            ListElement {
                text: qsTr("Compatible with OpenGL")
                option: "OpenGLCompatible"
            }
            ListElement {
                text: qsTr("Force integer metrics")
                option: "ForceIntegerMetrics"
            }
            ListElement {
                text: qsTr("No subpixel antialias")
                option: "NoSubpixelAntialias"
            }
            ListElement {
                text: qsTr("No font merging")
                option: "NoFontMerging"
            }
        }

        onCurrentIndexChanged: Charify.styleStrategy = cbxStyle.model.get(currentIndex).option
    }

    Label {
        text: qsTr("Foreground color")
    }
    RowLayout {
        Item {
            Layout.fillWidth: true
        }
        AK.ColorButton {
            currentColor: AkUtils.fromRgba(Charify.foregroundColor)
            title: qsTr("Choose the foreground color")
            showAlphaChannel: true

            onCurrentColorChanged: Charify.foregroundColor = AkUtils.toRgba(currentColor)
        }
    }

    Label {
        text: qsTr("Background color")
    }
    RowLayout {
        Item {
            Layout.fillWidth: true
        }
        AK.ColorButton {
            currentColor: AkUtils.fromRgba(Charify.backgroundColor)
            title: qsTr("Choose the background color")
            showAlphaChannel: true

            onCurrentColorChanged: Charify.backgroundColor = AkUtils.toRgba(currentColor)
        }
    }

    Label {
        text: qsTr("Reversed")
    }
    RowLayout {
        Item {
            Layout.fillWidth: true
        }
        Switch {
            checked: Charify.reversed

            onCheckedChanged: Charify.reversed = checked
        }
    }

    LABS.FontDialog {
        id: fontDialog
        title: qsTr("Please choose a font")
        font: Charify.font

        onAccepted: Charify.font = font
    }
}
