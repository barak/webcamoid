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
 * Email   : hipersayan DOT x AT gmail DOT com
 * Web-Site: http://github.com/hipersayanX/webcamoid
 */

#include <akutils.h>

#include "videocaptureelement.h"

VideoCaptureElement::VideoCaptureElement():
    AkMultimediaSourceElement()
{
    this->m_threadedRead = true;

    QObject::connect(&this->m_capture,
                     &Capture::error,
                     this,
                     &VideoCaptureElement::error);

    QObject::connect(&this->m_capture,
                     &Capture::webcamsChanged,
                     this,
                     &VideoCaptureElement::mediasChanged);

    QObject::connect(&this->m_capture,
                     &Capture::sizeChanged,
                     this,
                     &VideoCaptureElement::sizeChanged);

    QObject::connect(&this->m_capture,
                     &Capture::imageControlsChanged,
                     this,
                     &VideoCaptureElement::imageControlsChanged);

    QObject::connect(&this->m_capture,
                     &Capture::cameraControlsChanged,
                     this,
                     &VideoCaptureElement::cameraControlsChanged);

    QObject::connect(&this->m_timer,
                     &QTimer::timeout,
                     this,
                     &VideoCaptureElement::readFrame);
}

VideoCaptureElement::~VideoCaptureElement()
{
    this->setState(AkElement::ElementStateNull);
}

QObject *VideoCaptureElement::controlInterface(QQmlEngine *engine, const QString &controlId) const
{
    if (!engine)
        return NULL;

    // Load the UI from the plugin.
    QQmlComponent component(engine, QUrl(QStringLiteral("qrc:/VideoCapture/share/qml/main.qml")));

    if (component.isError()) {
        qDebug() << "Error in plugin "
                 << this->metaObject()->className()
                 << ":"
                 << component.errorString();

        return NULL;
    }

    // Create a context for the plugin.
    QQmlContext *context = new QQmlContext(engine->rootContext());
    context->setContextProperty("VideoCapture", (QObject *) this);
    context->setContextProperty("controlId", controlId);

    // Create an item with the plugin context.
    QObject *item = component.create(context);

    if (!item) {
        delete context;

        return NULL;
    }

    context->setParent(item);

    return item;
}

QStringList VideoCaptureElement::medias() const
{
    return this->m_capture.webcams();
}

QString VideoCaptureElement::media() const
{
    return this->m_capture.device();
}

QList<int> VideoCaptureElement::streams() const
{
    QList<int> streams;
    streams << 0;

    return streams;
}

int VideoCaptureElement::defaultStream(const QString &mimeType) const
{
    if (mimeType == "video/x-raw")
        return 0;

    return -1;
}

QString VideoCaptureElement::description(const QString &media) const
{
    return this->m_capture.description(media);
}

AkCaps VideoCaptureElement::caps(int stream) const
{
    if (stream != 0)
        return AkCaps();

    return this->m_capture.caps();
}

bool VideoCaptureElement::isCompressed(int stream) const
{
    if (stream != 0)
        return false;

    return this->m_capture.isCompressed();
}

QString VideoCaptureElement::ioMethod() const
{
    return this->m_capture.ioMethod();
}

int VideoCaptureElement::nBuffers() const
{
    return this->m_capture.nBuffers();
}

QVariantList VideoCaptureElement::availableSizes(const QString &webcam) const
{
    return this->m_capture.availableSizes(webcam);
}

QSize VideoCaptureElement::size(const QString &webcam) const
{
    return this->m_capture.size(webcam);
}

bool VideoCaptureElement::setSize(const QString &webcam, const QSize &size)
{
    bool running = this->m_timer.isActive();
    this->setState(AkElement::ElementStateNull);

    bool isSet = this->m_capture.setSize(webcam, size);

    if (running)
        this->setState(AkElement::ElementStatePlaying);

    return isSet;
}

bool VideoCaptureElement::resetSize(const QString &webcam)
{
    return this->m_capture.resetSize(webcam);
}

QVariantList VideoCaptureElement::imageControls(const QString &webcam) const
{
    return this->m_capture.imageControls(webcam);
}

bool VideoCaptureElement::setImageControls(const QString &webcam, const QVariantMap &imageControls) const
{
    return this->m_capture.setImageControls(webcam, imageControls);
}

bool VideoCaptureElement::resetImageControls(const QString &webcam) const
{
    return this->m_capture.resetImageControls(webcam);
}

QVariantList VideoCaptureElement::cameraControls(const QString &webcam) const
{
    return this->m_capture.cameraControls(webcam);
}

bool VideoCaptureElement::setCameraControls(const QString &webcam,
                                            const QVariantMap &cameraControls) const
{
    return this->m_capture.setCameraControls(webcam, cameraControls);
}

bool VideoCaptureElement::resetCameraControls(const QString &webcam) const
{
    return this->m_capture.resetCameraControls(webcam);
}

void VideoCaptureElement::sendPacket(VideoCaptureElement *element,
                                     const AkPacket &packet)
{
    AkPacket oPacket = element->m_convertVideo.convert(packet);

#if defined(Q_OS_WIN32)
    QImage oImage = AkUtils::packetToImage(oPacket).mirrored();

    emit element->oStream(AkUtils::imageToPacket(oImage, oPacket));
#else
    emit element->oStream(oPacket);
#endif
}

void VideoCaptureElement::setMedia(const QString &media)
{
    if (this->m_capture.device() == media)
        return;

    this->m_capture.setDevice(media);
    emit this->mediaChanged(media);
}

void VideoCaptureElement::setIoMethod(const QString &ioMethod)
{
    this->m_capture.setIoMethod(ioMethod);
}

void VideoCaptureElement::setNBuffers(int nBuffers)
{
    this->m_capture.setNBuffers(nBuffers);
}

void VideoCaptureElement::resetMedia()
{
    QString media = this->m_capture.device();
    this->m_capture.resetDevice();

    if (media != this->m_capture.device())
        emit this->mediaChanged(this->m_capture.device());
}

void VideoCaptureElement::resetIoMethod()
{
    this->m_capture.resetIoMethod();
}

void VideoCaptureElement::resetNBuffers()
{
    this->m_capture.resetNBuffers();
}

void VideoCaptureElement::reset(const QString &webcam)
{
    this->m_capture.reset(webcam);
}

bool VideoCaptureElement::setState(AkElement::ElementState state)
{
    AkElement::ElementState curState = this->state();

    switch (curState) {
    case AkElement::ElementStateNull: {
        switch (state) {
        case AkElement::ElementStatePaused:
            if (!this->m_capture.init())
                return false;

            return AkElement::setState(state);
        case AkElement::ElementStatePlaying:
            if (!this->m_capture.init())
                return false;

            this->m_timer.start();

            return AkElement::setState(state);
        default:
            break;
        }

        break;
    }
    case AkElement::ElementStatePaused: {
        switch (state) {
        case AkElement::ElementStateNull:
            this->m_capture.uninit();

            return AkElement::setState(state);
        case AkElement::ElementStatePlaying:
            this->m_timer.start();

            return AkElement::setState(state);
        default:
            break;
        }

        break;
    }
    case AkElement::ElementStatePlaying: {
        switch (state) {
        case AkElement::ElementStateNull:
            this->m_timer.stop();
            this->m_threadStatus.waitForFinished();
            this->m_capture.uninit();

            return AkElement::setState(state);
        case AkElement::ElementStatePaused:
            this->m_timer.stop();
            this->m_threadStatus.waitForFinished();

            return AkElement::setState(state);
        default:
            break;
        }

        break;
    }
    default:
        break;
    }

    return false;
}

void VideoCaptureElement::readFrame()
{
    AkPacket packet = this->m_capture.readFrame();

    if (!packet)
        return;

    if (!this->m_threadedRead) {
        emit this->oStream(packet);

        return;
    }

    if (!this->m_threadStatus.isRunning()) {
        this->m_curPacket = packet;

        this->m_threadStatus = QtConcurrent::run(&this->m_threadPool,
                                                 this->sendPacket,
                                                 this,
                                                 this->m_curPacket);
    }
}
