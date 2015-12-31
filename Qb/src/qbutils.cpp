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

#include "qbutils.h"

QbPacket QbUtils::imageToPacket(const QImage &image, const QbPacket &defaultPacket)
{
    QMap<QImage::Format, QString> imageToFormat;

    imageToFormat[QImage::Format_Mono] = "monob";
    imageToFormat[QImage::Format_Indexed8] = "gray";
    imageToFormat[QImage::Format_RGB32] = "bgr0";
    imageToFormat[QImage::Format_ARGB32] = "bgra";
    imageToFormat[QImage::Format_RGB16] = "rgb565le";
    imageToFormat[QImage::Format_RGB555] = "rgb555le";
    imageToFormat[QImage::Format_RGB888] = "bgr24";
    imageToFormat[QImage::Format_RGB444] = "rgb444le";

    if (!imageToFormat.contains(image.format()))
        return QbPacket();

    QbBufferPtr oBuffer(new char[image.byteCount()]);
    memcpy(oBuffer.data(), image.constBits(), image.byteCount());

    QbCaps caps(defaultPacket.caps());
    caps.setMimeType("video/x-raw");
    caps.setProperty("format", imageToFormat[image.format()]);
    caps.setProperty("width", image.width());
    caps.setProperty("height", image.height());

    QbPacket packet = defaultPacket;
    packet.setCaps(caps);
    packet.setBuffer(oBuffer);
    packet.setBufferSize(image.byteCount());

    return packet;
}

QImage QbUtils::packetToImage(const QbPacket &packet)
{
    if (packet.caps().mimeType() != "video/x-raw")
        return QImage();

    QMap<QString, QImage::Format> formatToImage;

    formatToImage["monob"] = QImage::Format_Mono;
    formatToImage["gray"] = QImage::Format_Indexed8;
    formatToImage["bgr0"] = QImage::Format_RGB32;
    formatToImage["bgra"] = QImage::Format_ARGB32;
    formatToImage["rgb565le"] = QImage::Format_RGB16;
    formatToImage["rgb555le"] = QImage::Format_RGB555;
    formatToImage["bgr24"] = QImage::Format_RGB888;
    formatToImage["rgb444le"] = QImage::Format_RGB444;

    QString format = packet.caps().property("format").toString();

    if (!formatToImage.contains(format))
        return QImage();

    int width = packet.caps().property("width").toInt();
    int height = packet.caps().property("height").toInt();

    QImage image((const uchar *) packet.buffer().data(),
                 width,
                 height,
                 formatToImage[format]);

    if (format == "gray")
        for (int i = 0; i < 256; i++)
            image.setColor(i, i);

    return image;
}

QString QbUtils::defaultSampleFormat(QAudioFormat::SampleType sampleType,
                                     int sampleSize,
                                     bool planar)
{
    QMap<QString, QString> sampleFormat;
    sampleFormat[QString("%1_%2_%3").arg(QAudioFormat::UnSignedInt).arg(8).arg(false)] = "u8";
    sampleFormat[QString("%1_%2_%3").arg(QAudioFormat::SignedInt).arg(16).arg(false)] = "s16";
    sampleFormat[QString("%1_%2_%3").arg(QAudioFormat::SignedInt).arg(32).arg(false)] = "s32";
    sampleFormat[QString("%1_%2_%3").arg(QAudioFormat::Float).arg(32).arg(false)] = "flt";
    sampleFormat[QString("%1_%2_%3").arg(QAudioFormat::Float).arg(64).arg(false)] = "dbl";
    sampleFormat[QString("%1_%2_%3").arg(QAudioFormat::UnSignedInt).arg(8).arg(true)] = "u8p";
    sampleFormat[QString("%1_%2_%3").arg(QAudioFormat::SignedInt).arg(16).arg(true)] = "s16p";
    sampleFormat[QString("%1_%2_%3").arg(QAudioFormat::SignedInt).arg(32).arg(true)] = "s32p";
    sampleFormat[QString("%1_%2_%3").arg(QAudioFormat::Float).arg(32).arg(true)] = "fltp";
    sampleFormat[QString("%1_%2_%3").arg(QAudioFormat::Float).arg(64).arg(true)] = "dblp";

    QString format = QString("%1_%2_%3").arg(sampleType).arg(sampleSize).arg(planar);

    if (sampleFormat.contains(format))
        return sampleFormat[format];

    return QString();
}

QString QbUtils::defaultChannelLayout(int nChannels)
{
    QStringList layouts;
    layouts << "mono"
            << "stereo"
            << "2.1"
            << "4.0"
            << "5.0"
            << "5.1"
            << "6.1"
            << "7.1";

    if (nChannels < 1
        || nChannels > layouts.size())
        return QString();

    return layouts[nChannels - 1];
}
