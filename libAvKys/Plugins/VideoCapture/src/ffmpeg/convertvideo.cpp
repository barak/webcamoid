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

#include <QMetaEnum>

#include "convertvideo.h"

ConvertVideo::ConvertVideo(QObject *parent):
    QObject(parent)
{
    this->m_scaleContext = NULL;
}

ConvertVideo::~ConvertVideo()
{
    if (this->m_scaleContext)
        sws_freeContext(this->m_scaleContext);
}

AkPacket ConvertVideo::convert(const AkPacket &packet)
{
    AkVideoPacket videoPacket(packet);

    // Convert input format.
    QString format = AkVideoCaps::pixelFormatToString(videoPacket.caps().format());
    AVPixelFormat iFormat = av_get_pix_fmt(format.toStdString().c_str());

    // Initialize rescaling context.
    this->m_scaleContext = sws_getCachedContext(this->m_scaleContext,
                                                videoPacket.caps().width(),
                                                videoPacket.caps().height(),
                                                iFormat,
                                                videoPacket.caps().width(),
                                                videoPacket.caps().height(),
                                                AV_PIX_FMT_BGRA,
                                                SWS_FAST_BILINEAR,
                                                NULL,
                                                NULL,
                                                NULL);

    if (!this->m_scaleContext)
        return AkPacket();

    // Create iPicture.
    AVFrame iFrame;
    memset(&iFrame, 0, sizeof(AVFrame));

    if (av_image_fill_arrays((uint8_t **) iFrame.data,
                         iFrame.linesize,
                         (const uint8_t *) videoPacket.buffer().constData(),
                         iFormat,
                         videoPacket.caps().width(),
                         videoPacket.caps().height(),
                         1) < 0)
        return AkPacket();

    // Create oPicture
    int frameSize = av_image_get_buffer_size(AV_PIX_FMT_BGRA,
                                             videoPacket.caps().width(),
                                             videoPacket.caps().height(),
                                             1);

    QByteArray oBuffer(frameSize, Qt::Uninitialized);
    AVFrame oFrame;
    memset(&oFrame, 0, sizeof(AVFrame));

    if (av_image_fill_arrays((uint8_t **) oFrame.data,
                         oFrame.linesize,
                         (const uint8_t *) oBuffer.constData(),
                         AV_PIX_FMT_BGRA,
                         videoPacket.caps().width(),
                         videoPacket.caps().height(),
                         1) < 0)
        return AkPacket();

    // Convert picture format
    sws_scale(this->m_scaleContext,
              iFrame.data,
              iFrame.linesize,
              0,
              videoPacket.caps().height(),
              oFrame.data,
              oFrame.linesize);

    // Create packet
    AkVideoPacket oPacket(packet);
    oPacket.caps().format() = AkVideoCaps::Format_bgra;
    oPacket.buffer() = oBuffer;

    return oPacket.toPacket();
}
