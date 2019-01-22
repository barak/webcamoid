/* Webcamoid, webcam capture application.
 * Copyright (C) 2016  Gonzalo Exequiel Pedone
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

#include <QImage>
#include <QQmlContext>
#include <QtMath>
#include <akvideopacket.h>

#include "shagadelicelement.h"

class ShagadelicElementPrivate
{
    public:
        QImage m_ripple;
        QImage m_spiral;
        QSize m_curSize;
        quint32 m_mask {0xffffff};
        int m_rx {0};
        int m_ry {0};
        int m_bx {0};
        int m_by {0};
        int m_rvx {0};
        int m_rvy {0};
        int m_bvx {0};
        int m_bvy {0};
        uchar m_phase {0};

        QImage makeRipple(const QSize &size) const;
        QImage makeSpiral(const QSize &size) const;
        void init(const QSize &size);
};

ShagadelicElement::ShagadelicElement(): AkElement()
{
    this->d = new ShagadelicElementPrivate;
}

ShagadelicElement::~ShagadelicElement()
{
    delete this->d;
}

quint32 ShagadelicElement::mask() const
{
    return this->d->m_mask;
}

QString ShagadelicElement::controlInterfaceProvide(const QString &controlId) const
{
    Q_UNUSED(controlId)

    return QString("qrc:/Shagadelic/share/qml/main.qml");
}

void ShagadelicElement::controlInterfaceConfigure(QQmlContext *context,
                                                  const QString &controlId) const
{
    Q_UNUSED(controlId)

    context->setContextProperty("Shagadelic", const_cast<QObject *>(qobject_cast<const QObject *>(this)));
    context->setContextProperty("controlId", this->objectName());
}

void ShagadelicElement::setMask(quint32 mask)
{
    if (this->d->m_mask == mask)
        return;

    this->d->m_mask = mask;
    emit this->maskChanged(mask);
}

void ShagadelicElement::resetMask()
{
    this->setMask(0xffffff);
}

AkPacket ShagadelicElement::iStream(const AkPacket &packet)
{
    AkVideoPacket videoPacket(packet);
    auto src = videoPacket.toImage();

    if (src.isNull())
        return AkPacket();

    src = src.convertToFormat(QImage::Format_ARGB32);
    QImage oFrame = QImage(src.size(), src.format());

    if (src.size() != this->d->m_curSize) {
        this->d->init(src.size());
        this->d->m_curSize = src.size();
    }

    for (int y = 0; y < src.height(); y++) {
        const QRgb *iLine = reinterpret_cast<const QRgb *>(src.constScanLine(y));
        QRgb *oLine = reinterpret_cast<QRgb *>(oFrame.scanLine(y));
        const quint8 *rLine = this->d->m_ripple.constScanLine(y + this->d->m_ry);
        const quint8 *gLine = this->d->m_spiral.constScanLine(y);
        const quint8 *bLine = this->d->m_ripple.constScanLine(y + this->d->m_by);

        for (int x = 0; x < src.width(); x++) {
            // Color saturation
            int r = qRed(iLine[x]) > 127? 255: 0;
            int g = qGreen(iLine[x]) > 127? 255: 0;
            int b = qBlue(iLine[x]) > 127? 255: 0;
            int a = qAlpha(iLine[x]);

            int pr = char(rLine[x + this->d->m_rx] + this->d->m_phase * 2) >> 7;
            int pg = char(gLine[x] + this->d->m_phase * 3) >> 7;
            int pb = char(bLine[x + this->d->m_by] - this->d->m_phase) >> 7;

            oLine[x] = qRgba(r, g, b, a) & qRgb(pr, pg, pb)
                       & (this->d->m_mask | 0xff000000);
        }
    }

    this->d->m_phase -= 8;

    if ((this->d->m_rx + this->d->m_rvx) < 0
        || (this->d->m_rx + this->d->m_rvx) >= src.width())
        this->d->m_rvx = -this->d->m_rvx;

    if ((this->d->m_ry + this->d->m_rvy) < 0
        || (this->d->m_ry + this->d->m_rvy) >= src.height())
        this->d->m_rvy = -this->d->m_rvy;

    if ((this->d->m_bx + this->d->m_bvx) < 0
        || (this->d->m_bx + this->d->m_bvx) >= src.width())
        this->d->m_bvx = -this->d->m_bvx;

    if ((this->d->m_by + this->d->m_bvy) < 0
        || (this->d->m_by + this->d->m_bvy) >= src.height())
        this->d->m_bvy = -this->d->m_bvy;

    this->d->m_rx += this->d->m_rvx;
    this->d->m_ry += this->d->m_rvy;
    this->d->m_bx += this->d->m_bvx;
    this->d->m_by += this->d->m_bvy;

    auto oPacket = AkVideoPacket::fromImage(oFrame, videoPacket).toPacket();
    akSend(oPacket)
}

QImage ShagadelicElementPrivate::makeRipple(const QSize &size) const
{
    QImage ripple(2 * size, QImage::Format_Grayscale8);

    for (int y = 0; y < ripple.height(); y++) {
        qreal yy = qreal(y) / size.width() - 1.0;
        auto oLine = reinterpret_cast<quint8 *>(ripple.scanLine(y));

        for (int x = 0; x < ripple.width(); x++) {
            qreal xx = qreal(x) / size.width() - 1.0;
            oLine[x] = uint(3000 * sqrt(xx * xx + yy * yy)) & 255;
        }
    }

    return ripple;
}

QImage ShagadelicElementPrivate::makeSpiral(const QSize &size) const
{
    QImage spiral(size, QImage::Format_Grayscale8);
    int yc = spiral.height() / 2;

    for (int y = 0; y < spiral.height(); y++) {
        qreal yy = qreal(y - yc) / spiral.width();
        auto oLine = reinterpret_cast<quint8 *>(spiral.scanLine(y));

        for (int x = 0; x < spiral.width(); x++) {
            qreal xx = qreal(x) / spiral.width() - 0.5;

            oLine[x] = uint(256 * 9 * atan2(xx, yy) / M_PI
                            + 1800 * sqrt(xx * xx + yy * yy))
                       & 255;
        }
    }

    return spiral;
}

void ShagadelicElementPrivate::init(const QSize &size)
{
    this->m_ripple = this->makeRipple(size);
    this->m_spiral = this->makeSpiral(size);

    this->m_rx = qrand() % size.width();
    this->m_ry = qrand() % size.height();
    this->m_bx = qrand() % size.width();
    this->m_by = qrand() % size.height();

    this->m_rvx = -2;
    this->m_rvy = -2;
    this->m_bvx = 2;
    this->m_bvy = 2;

    this->m_phase = 0;
}

#include "moc_shagadelicelement.cpp"
