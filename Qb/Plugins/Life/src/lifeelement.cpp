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

#include "lifeelement.h"

LifeElement::LifeElement(): QbElement()
{
    this->m_convert = QbElement::create("VCapsConvert");
    this->m_convert->setProperty("caps", "video/x-raw,format=bgr0");

    this->resetThreshold();
}

QObject *LifeElement::controlInterface(QQmlEngine *engine, const QString &controlId) const
{
    Q_UNUSED(controlId)

    if (!engine)
        return NULL;

    // Load the UI from the plugin.
    QQmlComponent component(engine, QUrl(QStringLiteral("qrc:/Life/share/qml/main.qml")));

    // Create a context for the plugin.
    QQmlContext *context = new QQmlContext(engine->rootContext());
    context->setContextProperty("Life", (QObject *) this);
    context->setContextProperty("controlId", this->objectName());

    // Create an item with the plugin context.
    QObject *item = component.create(context);
    context->setParent(item);

    return item;
}

int LifeElement::threshold() const
{
    return this->m_threshold / 7;
}

QImage LifeElement::imageBgSubtractUpdateY(const QImage &src)
{
    quint32 *p = (quint32 *) src.bits();
    short *q = (short *) this->m_background.bits();
    qint8 *d = (qint8 *) this->m_diff.bits();
    int videoArea = src.width() * src.height();

    for (int i = 0; i < videoArea; i++) {
        int r = ((*p) & 0xff0000) >> (16 - 1);
        int g = ((*p) & 0xff00) >> (8 - 2);
        int b = (*p) & 0xff;
        int v = (r + g + b) - (int) (*q);

        *q = (short) (r + g + b);
        *d = ((this->m_threshold + v) >> 24)
             | ((this->m_threshold - v) >> 24);

        p++;
        q++;
        d++;
    }

    return this->m_diff;
}

QImage LifeElement::imageDiffFilter(const QImage &diff)
{
    quint8 *src = (quint8 *) diff.bits();
    quint8 *dest = (quint8 *) this->m_diff2.bits() + diff.width() + 1;

    for (int y = 1; y < diff.height() - 1; y++) {
        uint sum1 = src[0] + src[diff.width()] + src[diff.width() * 2];
        uint sum2 = src[1] + src[diff.width() + 1] + src[diff.width() * 2 + 1];
        src += 2;

        for (int x = 1; x < diff.width() - 1; x++) {
            uint sum3 = src[0] + src[diff.width()] + src[diff.width() * 2];
            uint count = sum1 + sum2 + sum3;

            sum1 = sum2;
            sum2 = sum3;

            *dest++ = (0xff * 3 - count) >> 24;
            src++;
        }

        dest += 2;
    }

    return this->m_diff2;
}

void LifeElement::setThreshold(int threshold)
{
    threshold *= 7;

    if (threshold != this->m_threshold) {
        this->m_threshold = threshold;
        emit this->thresholdChanged();
    }
}

void LifeElement::resetThreshold()
{
    this->setThreshold(40);
}

void LifeElement::clearField()
{
    int videoArea = this->m_background.width()
                    * this->m_background.height();

    memset(this->m_field1, 0, videoArea);
}

QbPacket LifeElement::iStream(const QbPacket &packet)
{
    QbPacket iPacket = this->m_convert->iStream(packet);
    QImage src = QbUtils::packetToImage(iPacket);

    if (src.isNull())
        return QbPacket();

    int videoArea = src.width() * src.height();

    if (packet.caps() != this->m_caps) {
        this->m_background = QImage(src.width(), src.height(), QImage::Format_RGB32);
        this->m_diff = QImage(src.width(), src.height(), QImage::Format_Indexed8);
        this->m_diff2 = QImage(src.width(), src.height(), QImage::Format_Indexed8);

        this->m_field = QImage(src.width(), 2 * src.height(), QImage::Format_Indexed8);
        this->m_field1 = (quint8 *) this->m_field.bits();
        this->m_field2 = (quint8 *) this->m_field.bits() + videoArea;
        this->clearField();

        this->m_caps = packet.caps();
    }

    QImage oFrame = QImage(src.size(), src.format());

    quint32 *srcBits = (quint32 *) src.bits();
    quint32 *destBits = (quint32 *) oFrame.bits();

    this->imageDiffFilter(this->imageBgSubtractUpdateY(src));
    quint8 *p = this->m_diff2.bits();

    for (int x = 0; x < videoArea; x++)
        this->m_field1[x] |= p[x];

    p = this->m_field1 + 1;
    quint8 *q = this->m_field2 + src.width() + 1;
    destBits += src.width() + 1;
    srcBits += src.width() + 1;

    // each value of cell is 0 or 0xff. 0xff can be treated as -1, so
    // following equations treat each value as negative number.
    for (int y = 1; y < src.height() - 1; y++) {
        quint8 sum1 = 0;
        quint8 sum2 = p[0] + p[src.width()] + p[src.width() * 2];

        for (int x = 1; x < src.width() - 1; x++) {
            quint8 sum3 = p[1] + p[src.width() + 1] + p[src.width() * 2 + 1];
            quint8 sum = sum1 + sum2 + sum3;
            quint8 v = 0 - ((sum == 0xfd) | ((p[src.width()] != 0) & (sum == 0xfc)));
            *q++ = v;
            quint32 pix = (qint8) v;
            // pix = pix >> 8;
            *destBits++ = pix | *srcBits++;
            sum1 = sum2;
            sum2 = sum3;
            p++;
        }

        p += 2;
        q += 2;
        srcBits += 2;
        destBits += 2;
    }

    p = this->m_field1;
    this->m_field1 = this->m_field2;
    this->m_field2 = p;

    QbPacket oPacket = QbUtils::imageToPacket(oFrame, iPacket);
    qbSend(oPacket)
}
