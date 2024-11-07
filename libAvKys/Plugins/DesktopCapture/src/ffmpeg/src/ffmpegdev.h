/* Webcamoid, webcam capture application.
 * Copyright (C) 2023  Gonzalo Exequiel Pedone
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

#ifndef FFMPEGDEV_H
#define FFMPEGDEV_H

#include "screendev.h"

class FFmpegDevPrivate;

class FFmpegDev: public ScreenDev
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
    Q_PROPERTY(bool canCaptureCursor
               READ canCaptureCursor
               CONSTANT)
    Q_PROPERTY(bool canChangeCursorSize
               READ canChangeCursorSize
               CONSTANT)
    Q_PROPERTY(bool showCursor
               READ showCursor
               WRITE setShowCursor
               RESET resetShowCursor
               NOTIFY showCursorChanged)
    Q_PROPERTY(int cursorSize
               READ cursorSize
               WRITE setCursorSize
               RESET resetCursorSize
               NOTIFY cursorSizeChanged)

    public:
        FFmpegDev();
        ~FFmpegDev();

        Q_INVOKABLE AkFrac fps() const override;
        Q_INVOKABLE QStringList medias() override;
        Q_INVOKABLE QString media() const override;
        Q_INVOKABLE QList<int> streams() const override;
        Q_INVOKABLE int defaultStream(AkCaps::CapsType type) override;
        Q_INVOKABLE QString description(const QString &media) override;
        Q_INVOKABLE AkVideoCaps caps(int stream) override;
        Q_INVOKABLE bool canCaptureCursor() const override;
        Q_INVOKABLE bool canChangeCursorSize() const override;
        Q_INVOKABLE bool showCursor() const override;
        Q_INVOKABLE int cursorSize() const override;

    private:
        FFmpegDevPrivate *d;

    signals:
        void mediasChanged(const QStringList &medias);
        void mediaChanged(const QString &media);
        void streamsChanged(const QList<int> &streams);
        void loopChanged(bool loop);
        void fpsChanged(const AkFrac &fps);
        void sizeChanged(const QString &media, const QSize &size);
        void showCursorChanged(bool showCursor);
        void cursorSizeChanged(int cursorSize);
        void error(const QString &message);

    public slots:
        void setFps(const AkFrac &fps) override;
        void resetFps() override;
        void setMedia(const QString &media) override;
        void setShowCursor(bool showCursor) override;
        void setCursorSize(int cursorSize) override;
        void resetMedia() override;
        void setStreams(const QList<int> &streams) override;
        void resetStreams() override;
        void resetShowCursor() override;
        void resetCursorSize() override;
        bool init() override;
        bool uninit() override;
};

#endif // FFMPEGDEV_H
