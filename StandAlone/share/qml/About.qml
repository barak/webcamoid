/* Webcamoid, webcam capture application.
 * Copyright (C) 2015  Gonzalo Exequiel Pedone
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

Page {
    ScrollView {
        id: scrollView
        anchors.fill: parent
        contentHeight: clyProgramInfo.height
        clip: true

        ColumnLayout {
            id: clyProgramInfo
            width: scrollView.width

            RowLayout {
                Image {
                    fillMode: Image.PreserveAspectFit
                    Layout.minimumWidth:
                        AkUnit.create(128 * AkTheme.controlScale, "dp").pixels
                    Layout.minimumHeight:
                        AkUnit.create(128 * AkTheme.controlScale, "dp").pixels
                    Layout.maximumWidth:
                        AkUnit.create(128 * AkTheme.controlScale, "dp").pixels
                    Layout.maximumHeight:
                        AkUnit.create(128 * AkTheme.controlScale, "dp").pixels
                    source: "image://icons/webcamoid"
                    sourceSize: Qt.size(width, height)
                }

                ColumnLayout {
                    Label {
                        text: mediaTools.applicationName
                        font.bold: true
                        font.pointSize: 12
                    }
                    Label {
                        text: qsTr("Version %1").arg(mediaTools.applicationVersion)
                        font.bold: true
                    }
                    RowLayout {
                        visible: mediaTools.projectGitShortCommit.length > 0

                        Label {
                            //: Built from "short commit hash"
                            text: qsTr("Built from")
                        }
                        Button {
                            text: mediaTools.projectGitShortCommit
                            flat: true
                            Accessible.name:
                                qsTr("Built from %1").arg(mediaTools.projectGitShortCommit)
                            Accessible.description:
                                qsTr("Open the commit in your web browser")

                            onClicked: Qt.openUrlExternally(mediaTools.projectGitCommitUrl)
                        }
                    }
                    Label {
                        text: qsTr("Using Qt %1")
                                .arg(mediaTools.qtVersion)
                    }
                    Button {
                        text: qsTr("Website")
                        icon.source: "image://icons/internet"
                        Accessible.name: text
                        Accessible.description:
                            qsTr("Go to %1 website").arg(mediaTools.applicationName)

                        onClicked: Qt.openUrlExternally(mediaTools.projectUrl)
                    }
                }
            }

            Label {
                text: qsTr("Webcam capture application.")
            }
            Label {
                text: qsTr("A simple webcam application for picture and video capture.")
            }
            Label {
                text: mediaTools.copyrightNotice
            }
        }
    }
}
