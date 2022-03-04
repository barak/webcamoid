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

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlProperty>
#include <QQuickItem>
#include <QSettings>
#include <QStandardPaths>
#include <QtConcurrent>
#include <akaudiocaps.h>
#include <akcaps.h>
#include <akpacket.h>
#include <akplugin.h>
#include <akvideocaps.h>
#include <akplugininfo.h>
#include <akpluginmanager.h>

#ifdef Q_OS_WIN32
#include <windows.h>
#endif

#include "videolayer.h"
#include "clioptions.h"
#include "mediatools.h"
#include "updates.h"

#define DUMMY_OUTPUT_DEVICE ":dummyout:"

using ObjectPtr = QSharedPointer<QObject>;

class VideoLayerPrivate
{
    public:
        VideoLayer *self;
        QQmlApplicationEngine *m_engine {nullptr};
        QString m_videoInput;
        QStringList m_videoOutput;
        QStringList m_inputs;
        QMap<QString, QString> m_streams;
        AkAudioCaps m_inputAudioCaps;
        AkVideoCaps m_inputVideoCaps;
        AkElementPtr m_cameraCapture {akPluginManager->create<AkElement>("VideoSource/CameraCapture")};
        AkElementPtr m_desktopCapture {akPluginManager->create<AkElement>("VideoSource/DesktopCapture")};
        AkElementPtr m_uriCapture {akPluginManager->create<AkElement>("MultimediaSource/MultiSrc")};
        AkElementPtr m_cameraOutput {akPluginManager->create<AkElement>("VideoSink/VirtualCamera")};
        QString m_vcamDriver;
        QThreadPool m_threadPool;
        AkElement::ElementState m_state {AkElement::ElementStateNull};
        QString m_latestVersion;
        bool m_playOnStart {true};
        bool m_outputsAsInputs {false};

        explicit VideoLayerPrivate(VideoLayer *self);
        void connectSignals();
        AkElementPtr sourceElement(const QString &stream) const;
        QString sourceId(const QString &stream) const;
        QStringList cameras() const;
        QStringList desktops() const;
        QString cameraDescription(const QString &camera) const;
        QString desktopDescription(const QString &desktop) const;
        bool embedControls(const QString &where,
                           const AkElementPtr &element,
                           const QString &pluginId,
                           const QString &name) const;
        void setInputAudioCaps(const AkAudioCaps &audioCaps);
        void setInputVideoCaps(const AkVideoCaps &videoCaps);
        void loadProperties();
        void saveVideoInput(const QString &videoInput);
        void saveVideoOutput(const QString &videoOutput);
        void saveStreams(const QMap<QString, QString> &streams);
        void savePlayOnStart(bool playOnStart);
        void saveOutputsAsInputs(bool outputsAsInputs);
        inline QString vcamDownloadUrl() const;
};

VideoLayer::VideoLayer(QQmlApplicationEngine *engine, QObject *parent):
    QObject(parent)
{
    this->d = new VideoLayerPrivate(this);
    this->setQmlEngine(engine);
    this->d->connectSignals();
    this->d->loadProperties();
    this->d->m_latestVersion = this->currentVCamVersion();
    this->d->m_vcamDriver =
            akPluginManager->defaultPlugin("VideoSink/VirtualCamera/Impl/*").id();
    QObject::connect(akPluginManager,
                     &AkPluginManager::linksChanged,
                     this,
                     [this] (const AkPluginLinks &links) {
        if (links.contains("VideoSink/VirtualCamera/Impl/*")
            && links["VideoSink/VirtualCamera/Impl/*"] != this->d->m_vcamDriver) {
            this->d->m_vcamDriver = links["VideoSink/VirtualCamera/Impl/*"];
            emit this->vcamDriverChanged(this->d->m_vcamDriver);
            QString version;

            if (this->d->m_cameraOutput)
                version = this->d->m_cameraOutput->property("driverVersion").toString();

            emit this->currentVCamVersionChanged(version);
        }
    });
}

VideoLayer::~VideoLayer()
{
    this->setState(AkElement::ElementStateNull);
    delete this->d;
}

QString VideoLayer::videoInput() const
{
    return this->d->m_videoInput;
}

QStringList VideoLayer::videoOutput() const
{
    return this->d->m_videoOutput;
}

QStringList VideoLayer::inputs() const
{
    return this->d->m_inputs;
}

QStringList VideoLayer::outputs() const
{
    QStringList outputs;

    if (this->d->m_cameraOutput) {
        auto outs = this->d->m_cameraOutput->property("medias").toStringList();

        if (!outs.isEmpty())
            outputs << DUMMY_OUTPUT_DEVICE << outs;
    }

    return outputs;
}

AkAudioCaps VideoLayer::inputAudioCaps() const
{
    return this->d->m_inputAudioCaps;
}

AkVideoCaps VideoLayer::inputVideoCaps() const
{
    return this->d->m_inputVideoCaps;
}

AkVideoCaps::PixelFormatList VideoLayer::supportedOutputPixelFormats() const
{
    if (!this->d->m_cameraOutput)
        return {};

    AkVideoCaps::PixelFormatList pixelFormats;
    QMetaObject::invokeMethod(this->d->m_cameraOutput.data(),
                              "supportedOutputPixelFormats",
                              Q_RETURN_ARG(AkVideoCaps::PixelFormatList, pixelFormats));

    return pixelFormats;
}

AkVideoCaps::PixelFormat VideoLayer::defaultOutputPixelFormat() const
{
    if (!this->d->m_cameraOutput)
        return AkVideoCaps::Format_none;

    AkVideoCaps::PixelFormat pixelFormat;
    QMetaObject::invokeMethod(this->d->m_cameraOutput.data(),
                              "defaultOutputPixelFormat",
                              Q_RETURN_ARG(AkVideoCaps::PixelFormat, pixelFormat));

    return pixelFormat;
}

AkVideoCapsList VideoLayer::supportedOutputVideoCaps(const QString &device) const
{
    if (!this->d->m_cameraOutput)
        return {};

    AkVideoCapsList caps;
    QMetaObject::invokeMethod(this->d->m_cameraOutput.data(),
                              "outputCaps",
                              Q_RETURN_ARG(AkVideoCapsList, caps),
                              Q_ARG(QString, device));

    return caps;
}

AkElement::ElementState VideoLayer::state() const
{
    return this->d->m_state;
}

bool VideoLayer::playOnStart() const
{
    return this->d->m_playOnStart;
}

bool VideoLayer::outputsAsInputs() const
{
    return this->d->m_outputsAsInputs;
}

VideoLayer::InputType VideoLayer::deviceType(const QString &device) const
{
    if (this->d->cameras().contains(device))
        return InputCamera;

    if (this->d->desktops().contains(device))
        return InputDesktop;

    if (this->d->m_streams.contains(device))
        return InputStream;

    return InputUnknown;
}

QStringList VideoLayer::devicesByType(InputType type) const
{
    switch (type) {
    case InputCamera:
        return this->d->cameras();

    case InputDesktop:
        return this->d->desktops();

    case InputStream:
        return this->d->m_streams.keys();

    default:
        break;
    }

    return {};
}

QString VideoLayer::description(const QString &device) const
{
    if (device == DUMMY_OUTPUT_DEVICE)
        //: Disable video output, don't send the video to the output device.
        return tr("No Output");

    if (this->d->m_cameraOutput) {
        auto outputs = this->d->m_cameraOutput->property("medias").toStringList();

        if (outputs.contains(device)) {
            QString description;
            QMetaObject::invokeMethod(this->d->m_cameraOutput.data(),
                                      "description",
                                      Q_RETURN_ARG(QString, description),
                                      Q_ARG(QString, device));

            return description;
        }
    }

    if (this->d->cameras().contains(device))
        return this->d->cameraDescription(device);

    if (this->d->desktops().contains(device))
        return this->d->desktopDescription(device);

    if (this->d->m_streams.contains(device))
        return this->d->m_streams.value(device);

    return {};
}

QString VideoLayer::createOutput(VideoLayer::OutputType type,
                                 const QString &description,
                                 const AkVideoCapsList &formats)
{
    if (!this->d->m_cameraOutput || type != OutputVirtualCamera)
        return {};

    QString deviceId;
    QMetaObject::invokeMethod(this->d->m_cameraOutput.data(),
                              "createWebcam",
                              Q_RETURN_ARG(QString, deviceId),
                              Q_ARG(QString, description),
                              Q_ARG(AkVideoCapsList, formats));

    return deviceId;
}

QString VideoLayer::createOutput(VideoLayer::OutputType type,
                                 const QString &description,
                                 const QVariantList &formats)
{
    AkVideoCapsList fmts;

    for (auto &format: formats)
        fmts << format.value<AkVideoCaps>();

    return this->createOutput(type, description, fmts);
}

bool VideoLayer::editOutput(const QString &output,
                            const QString &description,
                            const AkVideoCapsList &formats)
{
    if (!this->d->m_cameraOutput)
        return {};

    bool result;
    QMetaObject::invokeMethod(this->d->m_cameraOutput.data(),
                              "editWebcam",
                              Q_RETURN_ARG(bool, result),
                              Q_ARG(QString, output),
                              Q_ARG(QString, description),
                              Q_ARG(AkVideoCapsList, formats));

    return result;
}

bool VideoLayer::removeOutput(const QString &output)
{
    if (!this->d->m_cameraOutput)
        return {};

    bool result;
    QMetaObject::invokeMethod(this->d->m_cameraOutput.data(),
                              "removeWebcam",
                              Q_RETURN_ARG(bool, result),
                              Q_ARG(QString, output));

    return result;
}

bool VideoLayer::removeAllOutputs()
{
    if (!this->d->m_cameraOutput)
        return {};

    bool result;
    QMetaObject::invokeMethod(this->d->m_cameraOutput.data(),
                              "removeAllWebcams",
                              Q_RETURN_ARG(bool, result));

    return result;
}

QString VideoLayer::inputError() const
{
    auto source = this->d->sourceElement(this->d->m_videoInput);

    if (!source)
        return {};

    QString error;
    QMetaObject::invokeMethod(source.data(),
                              "error",
                              Q_RETURN_ARG(QString, error));

    return error;
}

QString VideoLayer::outputError() const
{
    if (!this->d->m_cameraOutput)
        return {};

    QString error;
    QMetaObject::invokeMethod(this->d->m_cameraOutput.data(),
                              "error",
                              Q_RETURN_ARG(QString, error));

    return error;
}

bool VideoLayer::embedInputControls(const QString &where,
                                    const QString &device,
                                    const QString &name) const
{
    auto element = this->d->sourceElement(device);
    auto id = this->d->sourceId(device);

    return this->d->embedControls(where, element, id, name);
}

bool VideoLayer::embedOutputControls(const QString &where,
                                     const QString &device,
                                     const QString &name) const
{
    AkElementPtr element;

    if (this->d->m_cameraOutput) {
        auto outs = this->d->m_cameraOutput->property("medias").toStringList();

        if (outs.contains(device))
            element = this->d->m_cameraOutput;
   }

    return this->d->embedControls(where,
                                  element,
                                  "VideoSink/VirtualCamera",
                                  name);
}

void VideoLayer::removeInterface(const QString &where) const
{
    if (!this->d->m_engine)
        return;

    for (auto &obj: this->d->m_engine->rootObjects()) {
        auto item = obj->findChild<QQuickItem *>(where);

        if (!item)
            continue;

        auto childItems = item->childItems();

        for (auto &child: childItems) {
            child->setParentItem(nullptr);
            child->setParent(nullptr);
            delete child;
        }
    }
}

QList<quint64> VideoLayer::clientsPids() const
{
    if (!this->d->m_cameraOutput)
        return {};

    auto pids = this->d->m_cameraOutput->property("clientsPids");

    return pids.value<QList<quint64>>();
}

QString VideoLayer::clientExe(quint64 pid) const
{
    if (!this->d->m_cameraOutput)
        return {};

    QString exe;
    QMetaObject::invokeMethod(this->d->m_cameraOutput.data(),
                              "clientExe",
                              Q_RETURN_ARG(QString, exe),
                              Q_ARG(quint64, pid));

    return exe;
}

bool VideoLayer::driverInstalled() const
{
    if (!this->d->m_cameraOutput)
        return false;

    return this->d->m_cameraOutput->property("driverInstalled").toBool();
}

QString VideoLayer::picture() const
{
    if (!this->d->m_cameraOutput)
        return {};

    return this->d->m_cameraOutput->property("picture").toString();
}

QString VideoLayer::rootMethod() const
{
    if (!this->d->m_cameraOutput)
        return {};

    return this->d->m_cameraOutput->property("rootMethod").toString();
}

QStringList VideoLayer::availableRootMethods() const
{
    if (!this->d->m_cameraOutput)
        return {};

    return this->d->m_cameraOutput->property("availableRootMethods").toStringList();
}

bool VideoLayer::isVCamSupported() const
{
#if defined(Q_OS_WIN32) \
    || defined(Q_OS_OSX) \
    || (defined(Q_OS_LINUX) && ! defined(Q_OS_ANDROID))
    return true;
#else
    return false;
#endif
}

VideoLayer::VCamStatus VideoLayer::vcamInstallStatus() const
{
    auto akvcam = akPluginManager->create<QObject>("VideoSink/VirtualCamera/Impl/AkVCam");

    if (akvcam && akvcam->property("isInstalled").toBool())
        return VCamInstalled;

    if (this->d->m_cameraOutput
        && this->d->m_cameraOutput->property("driverInstalled").toBool())
        return VCamInstalledOther;

    return VCamNotInstalled;
}

QString VideoLayer::vcamDriver() const
{
    return this->d->m_vcamDriver;
}

QString VideoLayer::currentVCamVersion() const
{
    if (this->d->m_cameraOutput)
        return this->d->m_cameraOutput->property("driverVersion").toString();

    return {};
}

QString VideoLayer::vcamUpdateUrl() const
{
#if defined(Q_OS_WIN32) || defined(Q_OS_OSX)
    return {"https://api.github.com/repos/webcamoid/akvirtualcamera/releases/latest"};
#elif defined(Q_OS_LINUX)
    return {"https://api.github.com/repos/webcamoid/akvcam/releases/latest"};
#else
    return {};
#endif
}

QString VideoLayer::vcamDownloadUrl() const
{
#if defined(Q_OS_WIN32) || defined(Q_OS_OSX)
    return {"https://github.com/webcamoid/akvirtualcamera/releases/latest"};
#elif defined(Q_OS_LINUX)
    return {"https://github.com/webcamoid/akvcam/releases/latest"};
#else
    return {};
#endif
}

bool VideoLayer::applyPicture()
{
    if (!this->d->m_cameraOutput)
        return {};

    bool ok = false;
    QMetaObject::invokeMethod(this->d->m_cameraOutput.data(),
                              "applyPicture",
                              Q_RETURN_ARG(bool, ok));

    return ok;
}

void VideoLayer::setLatestVCamVersion(const QString &version)
{
    this->d->m_latestVersion = version;
}

bool VideoLayer::downloadVCam()
{
    if (!Updates::isOnline())
        return false;

    auto installerUrl = this->d->vcamDownloadUrl();

    if (installerUrl.isEmpty())
        return false;

    auto locations =
            QStandardPaths::standardLocations(QStandardPaths::TempLocation);

    if (locations.isEmpty())
        return false;

    auto outFile = QDir(locations[0]).filePath(QUrl(installerUrl).fileName());

    emit this->startVCamDownload(tr("Virtual Camera"),
                                    installerUrl,
                                    outFile);

    return true;
}

bool VideoLayer::executeVCamInstaller(const QString &installer)
{
    if (installer.isEmpty())
        return false;

    QFile(installer).setPermissions(QFileDevice::ReadOwner
                                    | QFileDevice::WriteOwner
                                    | QFileDevice::ExeOwner
                                    | QFileDevice::ReadUser
                                    | QFileDevice::WriteUser
                                    | QFileDevice::ExeUser
                                    | QFileDevice::ReadGroup
                                    | QFileDevice::ExeGroup
                                    | QFileDevice::ReadOther
                                    | QFileDevice::ExeOther);

    auto result =
            QtConcurrent::run(&this->d->m_threadPool, [this, installer] () {
        qDebug() << "Executing installer:" << installer;
        int exitCode = -1;
        QString errorString = "Can't execute installer";

#ifdef Q_OS_WIN32
        SHELLEXECUTEINFOA execInfo;
        memset(&execInfo, 0, sizeof(SHELLEXECUTEINFOA));
        execInfo.cbSize = sizeof(SHELLEXECUTEINFOA);
        execInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
        execInfo.hwnd = nullptr;
        execInfo.lpVerb = "runas";
        execInfo.lpFile = installer.toStdString().c_str();
        execInfo.lpParameters = "";
        execInfo.lpDirectory = "";
        execInfo.nShow = SW_HIDE;
        execInfo.hInstApp = nullptr;
        ShellExecuteExA(&execInfo);

        if (execInfo.hProcess) {
            WaitForSingleObject(execInfo.hProcess, INFINITE);

            DWORD dExitCode;
            GetExitCodeProcess(execInfo.hProcess, &dExitCode);
            CloseHandle(execInfo.hProcess);

            if (dExitCode == 0)
                errorString = "";
            else
                errorString = QString("Installer failed with code %1").arg(exitCode);

            exitCode = int(dExitCode);
        }
#elif defined(Q_OS_OSX)
        QProcess proc;
        proc.start("open", QStringList {"-W", installer});
        proc.waitForFinished(-1);
        exitCode = proc.exitCode();
        errorString = proc.errorString();
#else
        QProcess proc;
        proc.start(installer, QStringList {});
        proc.waitForFinished(-1);
        exitCode = proc.exitCode();
        errorString = proc.errorString();
#endif

        emit this->vcamInstallFinished(exitCode, errorString);
    });
    Q_UNUSED(result)

    return true;
}

void VideoLayer::checkVCamDownloadReady(const QString &url,
                                        const QString &filePath,
                                        DownloadManager::DownloadStatus status,
                                        const QString &error)
{
    auto installerUrl = this->d->vcamDownloadUrl();

    if (installerUrl.isEmpty())
        return;

    if (installerUrl != url)
        return;

    switch (status) {
    case DownloadManager::DownloadStatusFinished:
        emit this->vcamDownloadReady(filePath);

        break;

    case DownloadManager::DownloadStatusFailed:
        emit this->vcamDownloadFailed(error);

        break;

    default:
        break;
    }
}

void VideoLayer::setInputStream(const QString &stream,
                                const QString &description)
{
    if (stream.isEmpty()
        || description.isEmpty()
        || this->d->m_streams.value(stream) == description)
        return;

    this->d->m_streams[stream] = description;
    this->updateInputs();
    this->d->saveStreams(this->d->m_streams);
}

void VideoLayer::removeInputStream(const QString &stream)
{
    if (stream.isEmpty()
        || !this->d->m_streams.contains(stream))
        return;

    this->d->m_streams.remove(stream);
    this->updateInputs();
    this->d->saveStreams(this->d->m_streams);
}

void VideoLayer::setVideoInput(const QString &videoInput)
{
    if (this->d->m_videoInput == videoInput)
        return;

    this->d->m_videoInput = videoInput;
    emit this->videoInputChanged(videoInput);
    this->d->saveVideoInput(videoInput);
    this->updateCaps();
}

void VideoLayer::setVideoOutput(const QStringList &videoOutput)
{
    if (this->d->m_videoOutput == videoOutput)
        return;

    auto output = videoOutput.value(0);

    if (this->d->m_cameraOutput) {
        auto state = this->d->m_cameraOutput->state();
        this->d->m_cameraOutput->setState(AkElement::ElementStateNull);

        if (videoOutput.contains(DUMMY_OUTPUT_DEVICE)) {
            this->d->m_cameraOutput->setProperty("media", QString());
        } else {
            this->d->m_cameraOutput->setProperty("media", output);

            if (!output.isEmpty())
                this->d->m_cameraOutput->setState(state);
        }
    }

    this->d->m_videoOutput = videoOutput;
    emit this->videoOutputChanged(videoOutput);
    this->d->saveVideoOutput(output);
}

void VideoLayer::setState(AkElement::ElementState state)
{
    if (this->d->m_state == state)
        return;

    AkElementPtr source;

    if (this->d->cameras().contains(this->d->m_videoInput)) {
        if (this->d->m_desktopCapture)
            this->d->m_desktopCapture->setState(AkElement::ElementStateNull);

        if (this->d->m_uriCapture)
            this->d->m_uriCapture->setState(AkElement::ElementStateNull);

        source = this->d->m_cameraCapture;
    } else if (this->d->desktops().contains(this->d->m_videoInput)) {
        if (this->d->m_cameraCapture)
            this->d->m_cameraCapture->setState(AkElement::ElementStateNull);

        if (this->d->m_uriCapture)
            this->d->m_uriCapture->setState(AkElement::ElementStateNull);

        source = this->d->m_desktopCapture;
    } else if (this->d->m_streams.contains(this->d->m_videoInput)) {
        if (this->d->m_cameraCapture)
            this->d->m_cameraCapture->setState(AkElement::ElementStateNull);

        if (this->d->m_desktopCapture)
            this->d->m_desktopCapture->setState(AkElement::ElementStateNull);

        source = this->d->m_uriCapture;
    }

    if (source) {
        if (source->setState(state)
            || source->state() != this->d->m_state) {
            auto state = source->state();
            this->d->m_state = state;
            emit this->stateChanged(state);

            if (this->d->m_cameraOutput) {
                if (this->d->m_videoOutput.isEmpty()
                    || this->d->m_videoOutput.contains(DUMMY_OUTPUT_DEVICE))
                    this->d->m_cameraOutput->setState(AkElement::ElementStateNull);
                else
                    this->d->m_cameraOutput->setState(state);
            }
        }
    } else {
        if (this->d->m_state != AkElement::ElementStateNull) {
            this->d->m_state = AkElement::ElementStateNull;
            emit this->stateChanged(AkElement::ElementStateNull);

            if (this->d->m_cameraOutput)
                this->d->m_cameraOutput->setState(AkElement::ElementStateNull);
        }
    }
}

void VideoLayer::setPlayOnStart(bool playOnStart)
{
    if (this->d->m_playOnStart == playOnStart)
        return;

    this->d->m_playOnStart = playOnStart;
    emit this->playOnStartChanged(playOnStart);
    this->d->savePlayOnStart(playOnStart);
}

void VideoLayer::setOutputsAsInputs(bool outputsAsInputs)
{
    if (this->d->m_outputsAsInputs == outputsAsInputs)
        return;

    this->d->m_outputsAsInputs = outputsAsInputs;
    emit this->outputsAsInputsChanged(this->d->m_outputsAsInputs);
    this->updateInputs();
}

void VideoLayer::setPicture(const QString &picture)
{
    if (this->d->m_cameraOutput)
        this->d->m_cameraOutput->setProperty("picture", picture);
}

void VideoLayer::setRootMethod(const QString &rootMethod)
{
    if (this->d->m_cameraOutput)
        this->d->m_cameraOutput->setProperty("rootMethod", rootMethod);
}

void VideoLayer::resetVideoInput()
{
    this->setVideoInput({});
}

void VideoLayer::resetVideoOutput()
{
    this->setVideoOutput({});
}

void VideoLayer::resetState()
{
    this->setState(AkElement::ElementStateNull);
}

void VideoLayer::resetPlayOnStart()
{
    this->setPlayOnStart(true);
}

void VideoLayer::resetOutputsAsInputs()
{
    this->setOutputsAsInputs(false);
}

void VideoLayer::resetPicture()
{
    if (this->d->m_cameraOutput)
        QMetaObject::invokeMethod(this->d->m_cameraOutput.data(),
                                  "resetPicture");
}

void VideoLayer::resetRootMethod()
{
    if (this->d->m_cameraOutput)
        QMetaObject::invokeMethod(this->d->m_cameraOutput.data(),
                                  "resetRootMethod");
}

void VideoLayer::setQmlEngine(QQmlApplicationEngine *engine)
{
    if (this->d->m_engine == engine)
        return;

    this->d->m_engine = engine;

    if (engine) {
        engine->rootContext()->setContextProperty("videoLayer", this);
        qRegisterMetaType<InputType>("VideoInputType");
        qRegisterMetaType<OutputType>("VideoOutputType");
        qRegisterMetaType<VCamStatus>("VCamStatus");
        qmlRegisterType<VideoLayer>("Webcamoid", 1, 0, "VideoLayer");
    }
}

void VideoLayer::updateCaps()
{
    auto state = this->state();
    this->setState(AkElement::ElementStateNull);
    auto source = this->d->sourceElement(this->d->m_videoInput);

    if (this->d->m_cameraOutput)
        this->d->m_cameraOutput->setState(AkElement::ElementStateNull);

    AkCaps audioCaps;
    AkCaps videoCaps;

    if (source) {
        // Set the resource to play.
        source->setProperty("media", this->d->m_videoInput);

        // Update output caps.
        QList<int> streams;
        QMetaObject::invokeMethod(source.data(),
                                  "streams",
                                  Q_RETURN_ARG(QList<int>, streams));

        if (streams.isEmpty()) {
            int audioStream = -1;
            int videoStream = -1;

            // Find the defaults audio and video streams.
            QMetaObject::invokeMethod(source.data(),
                                      "defaultStream",
                                      Q_RETURN_ARG(int, audioStream),
                                      Q_ARG(QString, "audio/x-raw"));
            QMetaObject::invokeMethod(source.data(),
                                      "defaultStream",
                                      Q_RETURN_ARG(int, videoStream),
                                      Q_ARG(QString, "video/x-raw"));

            // Read streams caps.
            if (audioStream >= 0)
                QMetaObject::invokeMethod(source.data(),
                                          "caps",
                                          Q_RETURN_ARG(AkCaps, audioCaps),
                                          Q_ARG(int, audioStream));

            if (videoStream >= 0)
                QMetaObject::invokeMethod(source.data(),
                                          "caps",
                                          Q_RETURN_ARG(AkCaps, videoCaps),
                                          Q_ARG(int, videoStream));
        } else {
            for (const int &stream: streams) {
                AkCaps caps;
                QMetaObject::invokeMethod(source.data(),
                                          "caps",
                                          Q_RETURN_ARG(AkCaps, caps),
                                          Q_ARG(int, stream));

                if (caps.mimeType() == "audio/x-raw")
                    audioCaps = caps;
                else if (caps.mimeType() == "video/x-raw")
                    videoCaps = caps;
            }
        }
    }

    if (this->d->m_cameraOutput) {
        QMetaObject::invokeMethod(this->d->m_cameraOutput.data(),
                                  "clearStreams");
        QMetaObject::invokeMethod(this->d->m_cameraOutput.data(),
                                  "addStream",
                                  Q_ARG(int, 0),
                                  Q_ARG(AkCaps, videoCaps));

        if (!this->d->m_videoOutput.isEmpty() &&
            !this->d->m_videoOutput.contains(DUMMY_OUTPUT_DEVICE))
            this->d->m_cameraOutput->setState(state);
    }

    this->setState(state);
    this->d->setInputAudioCaps(audioCaps);
    this->d->setInputVideoCaps(videoCaps);
}

void VideoLayer::updateInputs()
{
    QStringList inputs;
    QMap<QString, QString> descriptions;

    // Read cameras
    auto cameras = this->d->cameras();
    inputs << cameras;

    for (auto &camera: cameras) {
        QString description;
        QMetaObject::invokeMethod(this->d->m_cameraCapture.data(),
                                  "description",
                                  Q_RETURN_ARG(QString, description),
                                  Q_ARG(QString, camera));
        descriptions[camera] = description;
    }

    // Read desktops
    auto desktops = this->d->desktops();
    inputs << desktops;

    for (auto &desktop: desktops) {
        QString description;
        QMetaObject::invokeMethod(this->d->m_desktopCapture.data(),
                                  "description",
                                  Q_RETURN_ARG(QString, description),
                                  Q_ARG(QString, desktop));
        descriptions[desktop] = description;
    }

    // Read streams
    inputs << this->d->m_streams.keys();

    for (auto it = this->d->m_streams.begin();
         it != this->d->m_streams.end();
         it++)
        descriptions[it.key()] = it.value();

    // Remove outputs to prevent self blocking.
    if (this->d->m_cameraOutput && !this->d->m_outputsAsInputs) {
        auto outputs =
                this->d->m_cameraOutput->property("medias").toStringList();

        for (auto &output: outputs)
            inputs.removeAll(output);
    }

    // Update inputs
    if (inputs != this->d->m_inputs) {
        this->d->m_inputs = inputs;
        emit this->inputsChanged(this->d->m_inputs);

        if (!this->d->m_inputs.contains(this->d->m_videoInput))
            this->setVideoInput(this->d->m_inputs.value(0));
    }
}

void VideoLayer::saveVirtualCameraRootMethod(const QString &rootMethod)
{
    QSettings config;
    config.beginGroup("VirtualCamera");
    config.setValue("rootMethod", rootMethod);
    config.endGroup();
}

AkPacket VideoLayer::iStream(const AkPacket &packet)
{
    if (this->d->m_cameraOutput
        && !this->d->m_videoOutput.isEmpty()
        && !this->d->m_videoOutput.contains(DUMMY_OUTPUT_DEVICE)
        && !this->d->m_videoOutput.contains(this->d->m_videoInput))
        this->d->m_cameraOutput->iStream(packet);

    return {};
}

VideoLayerPrivate::VideoLayerPrivate(VideoLayer *self):
    self(self)
{
}

void VideoLayerPrivate::connectSignals()
{
    if (this->m_cameraCapture) {
        QObject::connect(this->m_cameraCapture.data(),
                         SIGNAL(oStream(AkPacket)),
                         self,
                         SIGNAL(oStream(AkPacket)),
                         Qt::DirectConnection);
        QObject::connect(this->m_cameraCapture.data(),
                         SIGNAL(mediasChanged(QStringList)),
                         self,
                         SLOT(updateInputs()));
        QObject::connect(this->m_cameraCapture.data(),
                         SIGNAL(errorChanged(QString)),
                         self,
                         SIGNAL(inputErrorChanged(QString)));
        QObject::connect(this->m_cameraCapture.data(),
                         SIGNAL(streamsChanged(QList<int>)),
                         self,
                         SLOT(updateCaps()));
    }

    if (this->m_desktopCapture) {
        QObject::connect(this->m_desktopCapture.data(),
                         SIGNAL(oStream(AkPacket)),
                         self,
                         SIGNAL(oStream(AkPacket)),
                         Qt::DirectConnection);
        QObject::connect(this->m_desktopCapture.data(),
                         SIGNAL(mediasChanged(QStringList)),
                         self,
                         SLOT(updateInputs()));
        QObject::connect(this->m_desktopCapture.data(),
                         SIGNAL(error(QString)),
                         self,
                         SIGNAL(inputErrorChanged(QString)));
        QObject::connect(this->m_desktopCapture.data(),
                         SIGNAL(streamsChanged(QList<int>)),
                         self,
                         SLOT(updateCaps()));
    }

    if (this->m_uriCapture) {
        this->m_uriCapture->setProperty("objectName", "uriCapture");
        this->m_uriCapture->setProperty("loop", true);
        this->m_uriCapture->setProperty("audioAlign", true);

        QObject::connect(this->m_uriCapture.data(),
                         SIGNAL(oStream(AkPacket)),
                         self,
                         SIGNAL(oStream(AkPacket)),
                         Qt::DirectConnection);
        QObject::connect(this->m_uriCapture.data(),
                         SIGNAL(error(QString)),
                         self,
                         SIGNAL(inputErrorChanged(QString)));
        QObject::connect(this->m_uriCapture.data(),
                         SIGNAL(streamsChanged(QList<int>)),
                         self,
                         SLOT(updateCaps()));
    }

    if (this->m_cameraOutput) {
        QObject::connect(this->m_cameraOutput.data(),
                         SIGNAL(stateChanged(AkElement::ElementState)),
                         self,
                         SIGNAL(stateChanged(AkElement::ElementState)),
                         Qt::DirectConnection);
        QObject::connect(this->m_cameraOutput.data(),
                         SIGNAL(mediasChanged(QStringList)),
                         self,
                         SIGNAL(outputsChanged(QStringList)));
        QObject::connect(this->m_cameraOutput.data(),
                         SIGNAL(pictureChanged(QString)),
                         self,
                         SIGNAL(pictureChanged(QString)));
        QObject::connect(this->m_cameraOutput.data(),
                         SIGNAL(errorChanged(QString)),
                         self,
                         SIGNAL(outputErrorChanged(QString)));
        QObject::connect(this->m_cameraOutput.data(),
                         SIGNAL(rootMethodChanged(QString)),
                         self,
                         SIGNAL(rootMethodChanged(QString)),
                         Qt::DirectConnection);
        QObject::connect(this->m_cameraOutput.data(),
                         SIGNAL(rootMethodChanged(QString)),
                         self,
                         SLOT(saveVirtualCameraRootMethod(QString)));
    }
}

AkElementPtr VideoLayerPrivate::sourceElement(const QString &stream) const
{
    if (this->cameras().contains(stream))
        return this->m_cameraCapture;

    if (this->desktops().contains(stream))
        return this->m_desktopCapture;

    if (this->m_streams.contains(stream))
        return this->m_uriCapture;

    return {};
}

QString VideoLayerPrivate::sourceId(const QString &stream) const
{
    if (this->cameras().contains(stream))
        return {"VideoSource/CameraCapture"};

    if (this->desktops().contains(stream))
        return {"VideoSource/DesktopCapture"};

    if (this->m_streams.contains(stream))
        return {"MultimediaSource/MultiSrc"};

    return {};
}

QStringList VideoLayerPrivate::cameras() const
{
    if (!this->m_cameraCapture)
        return {};

    QStringList cameras;
    QMetaObject::invokeMethod(this->m_cameraCapture.data(),
                              "medias",
                              Q_RETURN_ARG(QStringList, cameras));
    return cameras;
}

QStringList VideoLayerPrivate::desktops() const
{
    if (!this->m_desktopCapture)
        return {};

    QStringList desktops;
    QMetaObject::invokeMethod(this->m_desktopCapture.data(),
                              "medias",
                              Q_RETURN_ARG(QStringList, desktops));
    return desktops;
}

QString VideoLayerPrivate::cameraDescription(const QString &camera) const
{
    if (!this->m_cameraCapture)
        return {};

    QString description;
    QMetaObject::invokeMethod(this->m_cameraCapture.data(),
                              "description",
                              Q_RETURN_ARG(QString, description),
                              Q_ARG(QString, camera));

    return description;
}

QString VideoLayerPrivate::desktopDescription(const QString &desktop) const
{
    if (!this->m_desktopCapture)
        return {};

    QString description;
    QMetaObject::invokeMethod(this->m_desktopCapture.data(),
                              "description",
                              Q_RETURN_ARG(QString, description),
                              Q_ARG(QString, desktop));

    return description;
}

bool VideoLayerPrivate::embedControls(const QString &where,
                                      const AkElementPtr &element,
                                      const QString &pluginId,
                                      const QString &name) const
{
    if (!element)
        return false;

    auto controlInterface = element->controlInterface(this->m_engine, pluginId);

    if (!controlInterface)
        return false;

    if (!name.isEmpty())
        controlInterface->setObjectName(name);

    for (auto &obj: this->m_engine->rootObjects()) {
        // First, find where to embed the UI.
        auto item = obj->findChild<QQuickItem *>(where);

        if (!item)
            continue;

        // Create an item with the plugin context.
        auto interfaceItem = qobject_cast<QQuickItem *>(controlInterface);

        // Finally, embed the plugin item UI in the desired place.
        interfaceItem->setParentItem(item);

        return true;
    }

    return false;
}

void VideoLayerPrivate::setInputAudioCaps(const AkAudioCaps &inputAudioCaps)
{
    if (this->m_inputAudioCaps == inputAudioCaps)
        return;

    this->m_inputAudioCaps = inputAudioCaps;
    emit self->inputAudioCapsChanged(inputAudioCaps);
}

void VideoLayerPrivate::setInputVideoCaps(const AkVideoCaps &inputVideoCaps)
{
    if (this->m_inputVideoCaps == inputVideoCaps)
        return;

    this->m_inputVideoCaps = inputVideoCaps;
    emit self->inputVideoCapsChanged(inputVideoCaps);
}

void VideoLayerPrivate::loadProperties()
{
    QSettings config;

    config.beginGroup("StreamConfigs");
    this->m_videoInput = config.value("stream").toString();
    this->m_playOnStart = config.value("playOnStart", true).toBool();

    int size = config.beginReadArray("uris");

    for (int i = 0; i < size; i++) {
        config.setArrayIndex(i);
        auto uri = config.value("uri").toString();
        auto description = config.value("description").toString();
        this->m_streams[uri] = description;
    }

    config.endArray();
    config.endGroup();

    config.beginGroup("VirtualCamera");
    this->m_outputsAsInputs = config.value("loopback", false).toBool();

    if (this->m_cameraOutput) {
        auto rootMethod =
                config.value("rootMethod",
                             this->m_cameraOutput->property("rootMethod")).toString();
        auto availableMethods =
                this->m_cameraOutput->property("availableRootMethods").toStringList();

        if (availableMethods.contains(rootMethod))
            this->m_cameraOutput->setProperty("rootMethod", rootMethod);

        auto streams = this->m_cameraOutput->property("medias").toStringList();
        auto stream = config.value("stream", streams.value(0)).toString();

        if (!streams.contains(stream))
            stream = streams.value(0);

        this->m_videoOutput = QStringList {stream};

        if (stream != DUMMY_OUTPUT_DEVICE)
            this->m_cameraOutput->setProperty("media", stream);
    }

    config.endGroup();

    self->updateInputs();
    self->updateCaps();
}

void VideoLayerPrivate::saveVideoInput(const QString &videoInput)
{
    QSettings config;
    config.beginGroup("StreamConfigs");
    config.setValue("stream", videoInput);
    config.endGroup();
}

void VideoLayerPrivate::saveVideoOutput(const QString &videoOutput)
{
    QSettings config;
    config.beginGroup("VirtualCamera");
    config.setValue("stream", videoOutput);
    config.endGroup();
}

void VideoLayerPrivate::saveStreams(const QMap<QString, QString> &streams)
{
    QSettings config;
    config.beginGroup("StreamConfigs");
    config.beginWriteArray("uris");

    int i = 0;

    for (auto it = streams.begin(); it != streams.end(); it++) {
        config.setArrayIndex(i);
        config.setValue("uri", it.key());
        config.setValue("description", it.value());
        i++;
    }

    config.endArray();
    config.endGroup();
}

void VideoLayerPrivate::savePlayOnStart(bool playOnStart)
{
    QSettings config;
    config.beginGroup("StreamConfigs");
    config.setValue("playOnStart", playOnStart);
    config.endGroup();
}

void VideoLayerPrivate::saveOutputsAsInputs(bool outputsAsInputs)
{
    QSettings config;
    config.beginGroup("VirtualCamera");
    config.setValue("loopback", outputsAsInputs);
    config.endGroup();
}

QString VideoLayerPrivate::vcamDownloadUrl() const
{
    if (this->m_latestVersion.isEmpty())
        return {};

#if defined(Q_OS_WIN32)
    return QString("https://github.com/webcamoid/akvirtualcamera/releases/download/%1/akvirtualcamera-windows-%1.exe")
           .arg(this->m_latestVersion);
#elif defined(Q_OS_OSX)
    return QString("https://github.com/webcamoid/akvirtualcamera/releases/download/%1/akvirtualcamera-mac-%1.pkg")
           .arg(this->m_latestVersion);
#elif defined(Q_OS_LINUX)
    return QString("https://github.com/webcamoid/akvcam/releases/download/%1/akvcam-linux-%1.run")
           .arg(this->m_latestVersion);
#else
    return {};
#endif
}

#include "moc_videolayer.cpp"
