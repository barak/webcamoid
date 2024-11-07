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

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

GridLayout {
    id: grdCameraControl
    columns: 3

    property variant controlParams: []
    property real value: 0
    property real minimumValue: 0
    property real maximumValue: 1
    property real stepSize: 1
    property variant model: []
    property int minimumLeftWidth: 0
    property int minimumRightWidth: 0
    readonly property alias leftWidth: lblControlName.width
    readonly property alias rightWidth: spbRange.width

    signal controlChanged(string controlName, variant value)

    onControlParamsChanged: {
        state = controlParams.length > 1? controlParams[1]: ""
        minimumValue = controlParams.length > 2? controlParams[2]: 0
        maximumValue = controlParams.length > 3? controlParams[3]: 1
        stepSize = controlParams.length > 4? controlParams[4]: 1
        model = controlParams.length > 7? controlParams[7]: []
        value = controlParams.length > 6? controlParams[6]: 0
        spbRange.value = value
    }

    Label {
        id: lblControlName
        text: controlParams.length > 0? controlParams[0]: ""
        Layout.minimumWidth: minimumLeftWidth
    }
    Slider {
        id: sldRange
        from: grdCameraControl.minimumValue
        to: grdCameraControl.maximumValue
        stepSize: grdCameraControl.stepSize
        value: grdCameraControl.value
        Layout.fillWidth: true
        visible: false
        Accessible.name: lblControlName.text

        onValueChanged: {
            if (visible) {
                spbRange.value = value
                grdCameraControl.controlChanged(controlParams.length > 0? controlParams[0]: "", value)
            }
        }
    }
    SpinBox {
        id: spbRange
        value: sldRange.value
        from: grdCameraControl.minimumValue
        to: grdCameraControl.maximumValue
        stepSize: grdCameraControl.stepSize
        //Layout.minimumWidth: minimumRightWidth
        visible: false
        editable: true
        Accessible.name: lblControlName.text

        onValueChanged: {
            if (visible)
                sldRange.value = value
        }
    }
    SpinBox {
        id: spbRangeFloat
        value: multiplier * sldRange.value
        from: multiplier * grdCameraControl.minimumValue
        to: multiplier * grdCameraControl.maximumValue
        stepSize: Math.round(multiplier * grdCameraControl.stepSize)
        //Layout.minimumWidth: minimumRightWidth
        visible: false
        editable: true
        Accessible.name: lblControlName.text

        readonly property int decimals: 2
        readonly property int multiplier: Math.pow(10, decimals)

        validator: DoubleValidator {
            bottom: Math.min(spbRangeFloat.from, spbRangeFloat.to)
            top:  Math.max(spbRangeFloat.from, spbRangeFloat.to)
        }
        textFromValue: function(value, locale) {
            return Number(value / multiplier).toLocaleString(locale, 'f', decimals)
        }
        valueFromText: function(text, locale) {
            return Number.fromLocaleString(locale, text) * multiplier
        }
        onValueModified: {
            if (visible)
                sldRange.value = value / multiplier
        }
    }

    RowLayout {
        id: chkBoolContainer
        Layout.columnSpan: 2
        Layout.fillWidth: true
        visible: false

        Label {
            Layout.fillWidth: true
        }
        Switch {
            id: chkBool
            checked: grdCameraControl.value !== 0
            Accessible.name: lblControlName.text

            onCheckedChanged: {
                if (visible)
                    grdCameraControl.controlChanged(controlParams.length > 0? controlParams[0]: "", checked? 1: 0)
            }
        }
    }

    ComboBox {
        id: cbxMenu
        model: grdCameraControl.model
        currentIndex: grdCameraControl.value
        Layout.fillWidth: true
        Layout.columnSpan: 2
        visible: false
        Accessible.description: lblControlName.text

        onCurrentIndexChanged: {
            if (visible)
                grdCameraControl.controlChanged(controlParams.length > 0? controlParams[0]: "", currentIndex)
        }
    }

    states: [
        State {
            name: "integer"

            PropertyChanges {
                target: sldRange
                visible: true
            }
            PropertyChanges {
                target: spbRange
                visible: true
            }
        },
        State {
            name: "integer64"

            PropertyChanges {
                target: sldRange
                visible: true
            }
            PropertyChanges {
                target: spbRange
                visible: true
            }
        },
        State {
            name: "float"

            PropertyChanges {
                target: sldRange
                visible: true
            }
            PropertyChanges {
                target: spbRangeFloat
                visible: true
            }
        },
        State {
            name: "boolean"

            PropertyChanges {
                target: chkBoolContainer
                visible: true
            }
        },
        State {
            name: "menu"

            PropertyChanges {
                target: cbxMenu
                visible: true
            }
        }
    ]
}
