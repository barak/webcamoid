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

#ifndef QTSCREENDEV_H
#define QTSCREENDEV_H

#include <QTimer>
#include <QThreadPool>
#include <QtConcurrent>
#include <QMutex>
#include <QDesktopWidget>
#include <ak.h>
#include <akvideopacket.h>

#include "screendev.h"

class QtScreenDev: public ScreenDev
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
    Q_PROPERTY(AkFrac fps
               READ fps
               WRITE setFps
               RESET resetFps
               NOTIFY fpsChanged)

    public:
        explicit QtScreenDev();
        ~QtScreenDev();

        Q_INVOKABLE AkFrac fps() const;
        Q_INVOKABLE QStringList medias();
        Q_INVOKABLE QString media() const;
        Q_INVOKABLE QList<int> streams() const;
        Q_INVOKABLE int defaultStream(const QString &mimeType);
        Q_INVOKABLE QString description(const QString &media);
        Q_INVOKABLE AkCaps caps(int stream);

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
        void setStreams(const QList<int> &streams);
        void resetStreams();
        bool init();
        bool uninit();

    private slots:
        void readFrame();
        void screenCountChanged(QScreen *screen);
        void srceenResized(int screen);
};

#endif // QTSCREENDEV_H
