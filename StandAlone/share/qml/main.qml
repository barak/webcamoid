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
import QtQuick.Window 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import Ak 1.0
import AkControls 1.0 as AK
import Webcamoid 1.0

ApplicationWindow {
    id: wdgMainWidget
    title: mediaTools.applicationName
           + " "
           + version()
           + " - "
           + videoLayer.description(videoLayer.videoInput)
    visible: true
    width: mediaTools.windowWidth
    height: mediaTools.windowHeight

    readonly property string filePrefix: Ak.platform() == "windows"?
                                             "file:///":
                                             "file://"

    function version()
    {
        if (mediaTools.isDailyBuild) {
            let versionStr = qsTr("Daily Build")

            if (mediaTools.projectGitShortCommit.length > 0)
                versionStr += " (" + mediaTools.projectGitShortCommit + ")"

            return versionStr
        }

        return mediaTools.applicationVersion
    }

    function savePhoto()
    {
        recording.takePhoto()
        recording.savePhoto(qsTr("%1/Picture %2.%3")
                            .arg(recording.imagesDirectory)
                            .arg(mediaTools.currentTime())
                            .arg(recording.imageFormat))
        photoPreviewSaveAnimation.start()
    }

    function snapshotToClipboard()
    {
        recording.takePhoto()

        if (recording.copyToClipboard())
            console.debug("Capture snapshot to Clipboard successful")
        else
            console.debug("Capture snapshot to Clipboard failed")
    }

    function pathToUrl(path)
    {
        if (path.length < 1)
            return ""

        return wdgMainWidget.filePrefix + path
    }

    function adjustControlScale()
    {
        let physicalWidth = width / Screen.pixelDensity
        let physicalHeight = height / Screen.pixelDensity

        if (physicalWidth <= 100 || physicalHeight <= 100)
            AkTheme.controlScale = 1.0;
        else
            AkTheme.controlScale = 1.6;
    }

    onWidthChanged: {
        adjustControlScale()
        mediaTools.windowWidth = width
    }
    onHeightChanged: {
        adjustControlScale()
        mediaTools.windowHeight = height
    }

    Component.onCompleted: {
        x = (Screen.width - mediaTools.windowWidth) / 2
        y = (Screen.height - mediaTools.windowHeight) / 2
    }

    Connections {
        target: mediaTools

        function onNewInstanceOpened()
        {
            wdgMainWidget.raise();
            wdgMainWidget.requestActivate()
        }
    }

    Connections {
        target: videoLayer

        function onVcamCliInstallStarted()
        {
            runCommandDialog.start()
            runCommandDialog.open()
        }

        function onVcamCliInstallLineReady(line)
        {
            runCommandDialog.writeLine(line)
        }

        function onVcamCliInstallFinished()
        {
            runCommandDialog.stop()
        }
    }

    VideoDisplay {
        id: videoDisplay
        objectName: "videoDisplay"
        visible: videoLayer.state == AkElement.ElementStatePlaying
        smooth: true
        anchors.fill: parent
    }
    Image {
        id: photoPreviewThumbnail
        source: pathToUrl(recording.lastPhotoPreview)
        sourceSize: Qt.size(width, height)
        cache: false
        smooth: true
        mipmap: true
        fillMode: Image.PreserveAspectFit
        x: k * AkUnit.create(16 * AkTheme.controlScale, "dp").pixels
        y: k * (controlsLayout.y
                + (controlsLayout.height - photoPreview.height) / 2)
        width: k * (photoPreview.width - parent.width) + parent.width
        height: k * (photoPreview.height - parent.height) + parent.height
        visible: false

        property real k: 0
    }
    Image {
        id: videoPreviewThumbnail
        source: pathToUrl(recording.lastVideoPreview)
        sourceSize: Qt.size(width, height)
        cache: false
        smooth: true
        mipmap: true
        fillMode: Image.PreserveAspectFit
        x: k * (parent.width
                - videoPreview.width
                - AkUnit.create(16 * AkTheme.controlScale, "dp").pixels)
        y: k * (controlsLayout.y
                + (controlsLayout.height - videoPreview.height) / 2)
        width: k * (videoPreview.width - parent.width) + parent.width
        height: k * (videoPreview.height - parent.height) + parent.height
        visible: false

        property real k: 0
    }
    Button {
        id: leftControls
        icon.source: "image://icons/menu"
        text: qsTr("Main menu")
        display: AbstractButton.IconOnly
        flat: true
        anchors.top: parent.top
        anchors.topMargin: AkUnit.create(16 * AkTheme.controlScale, "dp").pixels
        anchors.left: parent.left
        anchors.leftMargin: AkUnit.create(16 * AkTheme.controlScale, "dp").pixels
        ToolTip.visible: hovered
        ToolTip.text: text
        Accessible.name: text
        Accessible.description: qsTr("Open main menu")

        onClicked: settings.popup()
    }
    SettingsMenu {
        id: settings
        width: AkUnit.create(250 * AkTheme.controlScale, "dp").pixels

        onOpenAudioSettings: mainPanel.openAudioSettings()
        onOpenVideoSettings: mainPanel.openVideoSettings()
        onOpenVideoEffectsPanel: mainPanel.openVideoEffects()
        onOpenSettings: settingsDialog.open()
        onOpenDonationsDialog: Qt.openUrlExternally(mediaTools.projectDonationsUrl)
        onOpenAboutDialog: aboutDialog.open()
    }
    Button {
        id: rightControls
        icon.source: "image://icons/settings"
        text: qsTr("Image capture options")
        display: AbstractButton.IconOnly
        flat: true
        anchors.top: parent.top
        anchors.topMargin: AkUnit.create(16 * AkTheme.controlScale, "dp").pixels
        anchors.right: parent.right
        anchors.rightMargin: AkUnit.create(16 * AkTheme.controlScale, "dp").pixels
        ToolTip.visible: hovered
        ToolTip.text: text
        Accessible.name: text
        Accessible.description: qsTr("Open image capture options menu")
        enabled: videoLayer.state == AkElement.ElementStatePlaying
        visible: cameraControls.state == ""

        onClicked: localSettings.popup()
    }
    LocalSettingsMenu {
        id: localSettings
        width: AkUnit.create(250 * AkTheme.controlScale, "dp").pixels

        onCopyToClipboard: snapshotToClipboard()
        onOpenImageCaptureSettings: imageCaptureDialog.open()
    }
    RecordingNotice {
        anchors.top: parent.top
        anchors.topMargin: AkUnit.create(16 * AkTheme.controlScale, "dp").pixels
        anchors.horizontalCenter: parent.horizontalCenter
        visible: recording.state == AkElement.ElementStatePlaying
    }
    ColumnLayout {
        id: controlsLayout
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        Item {
            id: cameraControls
            Layout.margins:
                AkUnit.create(16 * AkTheme.controlScale, "dp").pixels
            height: AkUnit.create(64 * AkTheme.controlScale, "dp").pixels
            Layout.fillWidth: true

            readonly property real smallButton: AkUnit.create(48 * AkTheme.controlScale, "dp").pixels
            readonly property real bigButton: AkUnit.create(64 * AkTheme.controlScale, "dp").pixels
            readonly property real previewSize: AkUnit.create(48 * AkTheme.controlScale, "dp").pixels
            readonly property int animationTime: 200

            AK.ImageButton {
                id: photoPreview
                text: qsTr("Open last photo")
                icon.source: pathToUrl(recording.lastPhotoPreview)
                width: cameraControls.previewSize
                height: cameraControls.previewSize
                fillMode: AkColorizedImage.PreserveAspectCrop
                cache: false
                visible: photoPreview.status == Image.Ready
                y: (parent.height - height) / 2
                ToolTip.visible: hovered
                ToolTip.text: text
                Accessible.name: text
                Accessible.description: qsTr("Open last photo taken")

                onClicked: {
                    if (photoPreview.status == AkColorizedImage.Ready) {
                        let url = "" + photoPreview.icon.source

                        if (!url.startsWith(wdgMainWidget.filePrefix))
                            url = wdgMainWidget.filePrefix + url

                        Qt.openUrlExternally(url)
                    }
                }
            }
            RoundButton {
                id: photoButton
                icon.source: "image://icons/photo"
                width: cameraControls.bigButton
                height: cameraControls.bigButton
                x: (parent.width - width) / 2
                y: (parent.height - height) / 2
                ToolTip.visible: hovered
                ToolTip.text: qsTr("Take a photo")
                Accessible.name:
                    cameraControls.state == ""?
                        qsTr("Take a photo"):
                        qsTr("Image capture mode")
                Accessible.description:
                    cameraControls.state == ""?
                        qsTr("Make a capture and save it to an image file"):
                        qsTr("Put %1 in image capture mode").arg(mediaTools.applicationName)
                focus: true
                enabled: recording.state == AkElement.ElementStateNull
                         && (videoLayer.state == AkElement.ElementStatePlaying
                             || cameraControls.state == "Video")

                onClicked: {
                    if (cameraControls.state == "Video") {
                        cameraControls.state = ""
                    } else {
                        if (!imageCaptureDialog.useFlash
                            || videoLayer.deviceType(videoLayer.videoInput) != VideoLayer.InputCamera) {
                            savePhoto()

                            return
                        }

                        if (imageCaptureDialog.delay == 0) {
                            if (imageCaptureDialog.useFlash)
                                flash.shot()
                            else
                                savePhoto()

                            return
                        }

                        if (updateProgress.running) {
                            updateProgress.stop()
                            pgbPhotoShot.value = 0
                        } else {
                            pgbPhotoShot.start = new Date().getTime()
                            updateProgress.start()
                        }
                    }
                }
            }
            RoundButton {
                id: videoButton
                icon.source: recording.state == AkElement.ElementStateNull?
                                 "image://icons/video":
                                 "image://icons/record-stop"
                width: cameraControls.smallButton
                height: cameraControls.smallButton
                x: parent.width - width
                y: (parent.height - height) / 2
                ToolTip.visible: hovered
                ToolTip.text: recording.state == AkElement.ElementStateNull?
                                  qsTr("Record video"):
                                  qsTr("Stop video recording")
                Accessible.name:
                    cameraControls.state == ""?
                        qsTr("Video capture mode"):
                    recording.state == AkElement.ElementStateNull?
                        qsTr("Record video"):
                        qsTr("Stop video recording")
                Accessible.description:
                    cameraControls.state == ""?
                        qsTr("Put %1 in video recording mode").arg(mediaTools.applicationName):
                    recording.state == AkElement.ElementStateNull?
                        qsTr("Start recording to a video file"):
                        qsTr("Stop current video recording")
                enabled: videoLayer.state == AkElement.ElementStatePlaying
                         || cameraControls.state == ""

                onClicked: {
                    if (cameraControls.state == "") {
                        cameraControls.state = "Video"
                    } else if (recording.state == AkElement.ElementStateNull) {
                        recording.state = AkElement.ElementStatePlaying
                    } else {
                        recording.state = AkElement.ElementStateNull
                        videoPreviewSaveAnimation.start()
                    }
                }
            }
            AK.ImageButton {
                id: videoPreview
                text: qsTr("Open last video")
                icon.source: pathToUrl(recording.lastVideoPreview)
                width: 0
                height: 0
                fillMode: AkColorizedImage.PreserveAspectCrop
                cache: false
                visible: false
                x: parent.width - width
                y: (parent.height - height) / 2
                ToolTip.visible: hovered
                ToolTip.text: text
                Accessible.name: text
                Accessible.description: qsTr("Open last recorded video")

                onClicked: {
                    if (videoPreview.status == Image.Ready) {
                        let url = recording.lastVideo

                        if (!url.startsWith(wdgMainWidget.filePrefix))
                            url = wdgMainWidget.filePrefix + url

                        Qt.openUrlExternally(url)
                    }
                }
            }

            states: [
                State {
                    name: "Video"

                    PropertyChanges {
                        target: photoPreview
                        width: 0
                        height: 0
                        visible: false
                    }
                    PropertyChanges {
                        target: photoButton
                        width: cameraControls.smallButton
                        height: cameraControls.smallButton
                        x: 0
                    }
                    PropertyChanges {
                        target: videoButton
                        width: cameraControls.bigButton
                        height: cameraControls.bigButton
                        x: (parent.width - width) / 2
                    }
                    PropertyChanges {
                        target: videoPreview
                        width: cameraControls.previewSize
                        height: cameraControls.previewSize
                        visible: true
                    }
                }
            ]

            transitions: Transition {
                PropertyAnimation {
                    target: photoPreview
                    properties: "width,height,visible"
                    duration: cameraControls.animationTime
                }
                PropertyAnimation {
                    target: photoButton
                    properties: "radius,x"
                    duration: cameraControls.animationTime
                }
                PropertyAnimation {
                    target: videoButton
                    properties: "radius,x"
                    duration: cameraControls.animationTime
                }
                PropertyAnimation {
                    target: videoPreview
                    properties: "width,height,visible"
                    duration: cameraControls.animationTime
                }
            }
        }
        ProgressBar {
            id: pgbPhotoShot
            Layout.fillWidth: true
            visible: updateProgress.running

            property double start: 0

            onValueChanged: {
                if (value >= 1) {
                    updateProgress.stop()
                    value = 0

                    if (imageCaptureDialog.useFlash)
                        flash.shot()
                    else
                        savePhoto()
                }
            }
        }
    }
    MainPanel {
        id: mainPanel

        onOpenErrorDialog: videoOutputError.openError(title, message)
        onOpenVideoEffectsDialog: videoEffectsDialog.open()
    }

    SequentialAnimation {
        id: photoPreviewSaveAnimation

        PropertyAnimation {
            target: photoPreviewThumbnail
            property: "k"
            to: 0
            duration: 0
        }
        PropertyAnimation {
            target: photoPreview
            property: "visible"
            to: false
            duration: 0
        }
        PropertyAnimation {
            target: photoPreviewThumbnail
            property: "visible"
            to: true
            duration: 0
        }
        PropertyAnimation {
            target: photoPreviewThumbnail
            property: "k"
            to: 1
            duration: 500
        }
        PropertyAnimation {
            target: photoPreviewThumbnail
            property: "visible"
            to: false
            duration: 0
        }
        PropertyAnimation {
            target: photoPreview
            property: "visible"
            to: true
            duration: 0
        }
    }
    SequentialAnimation {
        id: videoPreviewSaveAnimation

        PropertyAnimation {
            target: videoPreviewThumbnail
            property: "k"
            to: 0
            duration: 0
        }
        PropertyAnimation {
            target: videoPreview
            property: "visible"
            to: false
            duration: 0
        }
        PropertyAnimation {
            target: videoPreviewThumbnail
            property: "visible"
            to: true
            duration: 0
        }
        PropertyAnimation {
            target: videoPreviewThumbnail
            property: "k"
            to: 1
            duration: 500
        }
        PropertyAnimation {
            target: videoPreviewThumbnail
            property: "visible"
            to: false
            duration: 0
        }
        PropertyAnimation {
            target: videoPreview
            property: "visible"
            to: true
            duration: 0
        }
    }
    Timer {
        id: updateProgress
        interval: 100
        repeat: true

        onTriggered: {
            pgbPhotoShot.value = (new Date().getTime() - pgbPhotoShot.start)
                                 / imageCaptureDialog.delay
        }
    }
    Flash {
        id: flash

        onShotStarted: {
            if (isHardwareFlash)
                videoLayer.flashMode = VideoLayer.FlashMode_Torch
        }
        onTriggered: savePhoto()
        onShotFinished: {
            if (isHardwareFlash)
                videoLayer.flashMode = VideoLayer.FlashMode_Off
        }
    }
    RunCommandDialog {
        id: runCommandDialog
        title: qsTr("Installing virtual camera")
        message: qsTr("Running commands")
        anchors.centerIn: Overlay.overlay
    }
    ImageCaptureDialog {
        id: imageCaptureDialog
        anchors.centerIn: Overlay.overlay
    }
    VideoEffectsDialog {
        id: videoEffectsDialog
        width: parent.width
        height: parent.height
    }
    SettingsDialog {
        id: settingsDialog
        width: parent.width
        height: parent.height
    }
    VideoOutputError {
        id: videoOutputError
        anchors.centerIn: Overlay.overlay
    }
    UpdatesDialog {
        id: updatesDialog
        anchors.centerIn: Overlay.overlay
    }
    AboutDialog {
        id: aboutDialog
        anchors.centerIn: Overlay.overlay
    }
}
