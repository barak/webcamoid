/* Webcamoid, webcam capture application.
 * Copyright (C) 2011-2017  Gonzalo Exequiel Pedone
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

#ifndef DESKTOPCAPTUREELEMENT_H
#define DESKTOPCAPTUREELEMENT_H

#include <QThreadPool>
#include <QtConcurrent>
#include <akmultimediasourceelement.h>

#include "screendev.h"

typedef QSharedPointer<ScreenDev> ScreenDevPtr;

class DesktopCaptureElement: public AkMultimediaSourceElement
{
    Q_OBJECT
    Q_PROPERTY(QStringList medias
               READ medias
               NOTIFY mediasChanged)
    Q_PROPERTY(QString media
               READ media
               WRITE setMedia
               RESET resetMedia
               NOTIFY mediaChanged)
    Q_PROPERTY(QList<int> streams
               READ streams
               WRITE setStreams
               RESET resetStreams
               NOTIFY streamsChanged)
    Q_PROPERTY(bool loop
               READ loop
               WRITE setLoop
               RESET resetLoop
               NOTIFY loopChanged)
    Q_PROPERTY(AkFrac fps
               READ fps
               WRITE setFps
               RESET resetFps
               NOTIFY fpsChanged)
    Q_PROPERTY(QString captureLib
               READ captureLib
               WRITE setCaptureLib
               RESET resetCaptureLib
               NOTIFY captureLibChanged)

    public:
        explicit DesktopCaptureElement();
        ~DesktopCaptureElement();

        Q_INVOKABLE AkFrac fps() const;
        Q_INVOKABLE QStringList medias();
        Q_INVOKABLE QString media() const;
        Q_INVOKABLE QList<int> streams() const;
        Q_INVOKABLE int defaultStream(const QString &mimeType);
        Q_INVOKABLE QString description(const QString &media);
        Q_INVOKABLE AkCaps caps(int stream);
        Q_INVOKABLE QString captureLib() const;

    private:
        ScreenDevPtr m_screenCapture;

    protected:
        QString controlInterfaceProvide(const QString &controlId) const;
        void controlInterfaceConfigure(QQmlContext *context,
                                       const QString &controlId) const;

    signals:
        void mediasChanged(const QStringList &medias);
        void mediaChanged(const QString &media);
        void streamsChanged(const QList<int> &streams);
        void loopChanged(bool loop);
        void fpsChanged(const AkFrac &fps);
        void sizeChanged(const QString &media, const QSize &size);
        void error(const QString &message);
        void captureLibChanged(const QString &captureLib);

    public slots:
        void setFps(const AkFrac &fps);
        void resetFps();
        void setMedia(const QString &media);
        void resetMedia();
        void setCaptureLib(const QString &captureLib);
        void resetCaptureLib();
        bool setState(AkElement::ElementState state);

    private slots:
        void captureLibUpdated(const QString &captureLib);
};

#endif // DESKTOPCAPTUREELEMENT_H
