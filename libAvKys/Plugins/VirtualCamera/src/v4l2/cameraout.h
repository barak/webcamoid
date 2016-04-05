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

#ifndef CAMERAOUT_H
#define CAMERAOUT_H

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <QFileSystemWatcher>
#include <QDir>
#include <QSize>

#include <akpacket.h>
#include <akvideocaps.h>

class CameraOut: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList webcams
               READ webcams
               NOTIFY webcamsChanged)
    Q_PROPERTY(QString device
               READ device
               WRITE setDevice
               RESET resetDevice
               NOTIFY deviceChanged)
    Q_PROPERTY(int streamIndex
               READ streamIndex)
    Q_PROPERTY(AkCaps caps
               READ caps)

    public:
        explicit CameraOut();
        ~CameraOut();

        Q_INVOKABLE QStringList webcams() const;
        Q_INVOKABLE QString device() const;
        Q_INVOKABLE int streamIndex() const;
        Q_INVOKABLE AkCaps caps() const;
        Q_INVOKABLE QString description(const QString &webcam) const;
        Q_INVOKABLE void writeFrame(const AkPacket &frame);

    private:
        QStringList m_webcams;
        QString m_device;
        int m_streamIndex;
        AkCaps m_caps;
        QFileSystemWatcher *m_fsWatcher;
        int m_fd;

        inline int xioctl(int fd, int request, void *arg) const
        {
            int r = -1;

            forever {
                r = ioctl(fd, request, arg);

                if (r != -1 || errno != EINTR)
                    break;
            }

            return r;
        }

    signals:
        void webcamsChanged(const QStringList &webcams) const;
        void deviceChanged(const QString &device);
        void error(const QString &message);

    public slots:
        bool init(int streamIndex, const AkCaps &caps);
        void uninit();
        void setDevice(const QString &device);
        void resetDevice();

    private slots:
        void onDirectoryChanged(const QString &path);
};

#endif // CAMERAOUT_H
