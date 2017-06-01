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

#include <QQmlComponent>
#include <QQmlContext>
#include <QTimer>
#include <QThreadPool>
#include <QtConcurrent>
#include <QDesktopWidget>

#include <ak.h>
#include <akmultimediasourceelement.h>

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

    public:
        explicit DesktopCaptureElement();
        ~DesktopCaptureElement();

        Q_INVOKABLE QObject *controlInterface(QQmlEngine *engine,
                                              const QString &controlId) const;

        Q_INVOKABLE AkFrac fps() const;

        Q_INVOKABLE QStringList medias() const;
        Q_INVOKABLE QString media() const;
        Q_INVOKABLE QList<int> streams() const;

        Q_INVOKABLE int defaultStream(const QString &mimeType) const;
        Q_INVOKABLE QString description(const QString &media) const;
        Q_INVOKABLE AkCaps caps(int stream) const;

    private:
        AkFrac m_fps;
        QString m_curScreen;
        int m_curScreenNumber;
        qint64 m_id;
        bool m_threadedRead;
        QTimer m_timer;
        QThreadPool m_threadPool;
        QFuture<void> m_threadStatus;
        QMutex m_mutex;
        AkPacket m_curPacket;

        void sendPacket(const AkPacket &packet);

    signals:
        void mediasChanged(const QStringList &medias);
        void mediaChanged(const QString &media);
        void streamsChanged(const QList<int> &streams);
        void loopChanged(bool loop);
        void fpsChanged(const AkFrac &fps);
        void sizeChanged(const QString &media, const QSize &size);
        void error(const QString &message);

    public slots:
        void setFps(const AkFrac &fps);
        void resetFps();
        void setMedia(const QString &media);
        void resetMedia();
        bool setState(AkElement::ElementState state);

    private slots:
        void readFrame();
        void screenCountChanged(QScreen *screen);
        void srceenResized(int screen);
};

#endif // DESKTOPCAPTUREELEMENT_H
