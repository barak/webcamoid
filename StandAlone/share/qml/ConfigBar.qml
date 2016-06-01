/* Webcamoid, webcam capture application.
 * Copyright (C) 2011-2016  Gonzalo Exequiel Pedone
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

import QtQuick 2.5
import QtQuick.Controls 1.4

Rectangle {
    id: recConfigBar
    color: Qt.rgba(0, 0, 0, 0)
    clip: true
    width: 200
    height: 400

    property string option: ""

    OptionList {
        id: optConfigs
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.top: parent.top
        textRole: "description"
        model: ListModel {
            id: lstConfigs

            ListElement {
                option: "audio"
                description: qsTr("Audio")
            }
            ListElement {
                option: "output"
                description: qsTr("Output")
            }
            ListElement {
                option: "general"
                description: qsTr("General Options")
            }
        }

        onCurrentIndexChanged: recConfigBar.option = lstConfigs.get(optConfigs.currentIndex).option
    }
}
