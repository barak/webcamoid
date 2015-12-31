/* Webcamoid, webcam capture application.
 * Copyright (C) 2011-2015  Gonzalo Exequiel Pedone
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

#ifndef VIDEOSYNCELEMENT_H
#define VIDEOSYNCELEMENT_H

#include <cmath>
#include <QMutex>
#include <QWaitCondition>
#include <QTimer>
#include <QElapsedTimer>
#include <QQueue>
#include <qb.h>

#include "thread.h"

// no AV sync correction is done if below the minimum AV sync threshold
#define AV_SYNC_THRESHOLD_MIN 0.01

// AV sync correction is done if above the maximum AV sync threshold
#define AV_SYNC_THRESHOLD_MAX 0.1

// If a frame duration is longer than this, it will not be duplicated to compensate AV sync
#define AV_SYNC_FRAMEDUP_THRESHOLD 0.1

// no AV correction is done if too big error
#define AV_NOSYNC_THRESHOLD 10.0

class VideoSyncElement: public QbElement
{
    Q_OBJECT

    Q_PROPERTY(int maxQueueSize
               READ maxQueueSize
               WRITE setMaxQueueSize
               RESET resetMaxQueueSize
               NOTIFY maxQueueSizeChanged)

    Q_PROPERTY(bool showLog
               READ showLog
               WRITE setShowLog
               RESET resetShowLog
               NOTIFY showLogChanged)

    public:
        explicit VideoSyncElement();
        ~VideoSyncElement();

        Q_INVOKABLE int maxQueueSize() const;
        Q_INVOKABLE bool showLog() const;

    protected:
        void stateChange(QbElement::ElementState from, QbElement::ElementState to);

    private:
        int m_maxQueueSize;
        bool m_showLog;

        Thread *m_outputThread;
        bool m_run;

        QMutex m_mutex;
        QWaitCondition m_queueNotEmpty;
        QWaitCondition m_queueNotFull;
        QQueue<QbPacket> m_queue;

        QTimer m_timer;
        QElapsedTimer m_elapsedTimer;
        double m_timeDrift;
        double m_lastPts;

        void printLog(const QbPacket &packet, double diff);

    public slots:
        void setClock(double clock);
        void setMaxQueueSize(int maxQueueSize);
        void setShowLog(bool showLog);
        void resetMaxQueueSize();
        void resetShowLog();
        void processFrame();
        void init();
        void uninit();

        QbPacket iStream(const QbPacket &packet);

    signals:
        void maxQueueSizeChanged(int maxQueueSize);
        void showLogChanged(bool showLog);
};

#endif // VIDEOSYNCELEMENT_H
