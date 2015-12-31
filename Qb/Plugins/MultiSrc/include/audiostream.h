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

#ifndef AUDIOSTREAM_H
#define AUDIOSTREAM_H

#include "abstractstream.h"

class AudioStream: public AbstractStream
{
    Q_OBJECT
    Q_PROPERTY(bool align
               READ align
               WRITE setAlign
               RESET resetAlign)

    public:
        explicit AudioStream(const AVFormatContext *formatContext=NULL,
                             uint index=-1, qint64 id=-1, bool noModify=false,
                             QObject *parent=NULL);

        Q_INVOKABLE bool align() const;
        Q_INVOKABLE QbCaps caps() const;

    protected:
        void processPacket(AVPacket *packet);

    private:
        bool m_align;
        bool m_fst;
        qint64 m_pts;
        qint64 m_duration;

    public slots:
        void setAlign(bool align);
        void resetAlign();
};

#endif // AUDIOSTREAM_H
