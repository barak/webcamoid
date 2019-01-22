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

#include <QtMath>
#include <QPainter>
#include <QQmlContext>
#include <akvideopacket.h>

#include "radioactiveelement.h"

using RadiationModeMap = QMap<RadioactiveElement::RadiationMode, QString>;

inline RadiationModeMap initRadiationModeMap()
{
    RadiationModeMap radiationModeToStr = {
        {RadioactiveElement::RadiationModeSoftNormal, "softNormal"},
        {RadioactiveElement::RadiationModeHardNormal, "hardNormal"},
        {RadioactiveElement::RadiationModeSoftColor , "softColor" },
        {RadioactiveElement::RadiationModeHardColor , "hardColor" }
    };

    return radiationModeToStr;
}

Q_GLOBAL_STATIC_WITH_ARGS(RadiationModeMap, radiationModeToStr, (initRadiationModeMap()))

class RadioactiveElementPrivate
{
    public:
        QSize m_frameSize;
        QImage m_prevFrame;
        QImage m_blurZoomBuffer;
        AkElementPtr m_blurFilter;
        qreal m_zoom {1.1};
        RadioactiveElement::RadiationMode m_mode {RadioactiveElement::RadiationModeSoftNormal};
        int m_threshold {31};
        int m_lumaThreshold {95};
        int m_alphaDiff {-8};
        QRgb m_radColor {qRgb(0, 255, 0)};

        QImage imageDiff(const QImage &img1,
                         const QImage &img2,
                         int threshold,
                         int lumaThreshold,
                         QRgb radColor,
                         RadioactiveElement::RadiationMode mode);
        QImage imageAlphaDiff(const QImage &src, int alphaDiff);
};

RadioactiveElement::RadioactiveElement(): AkElement()
{
    this->d = new RadioactiveElementPrivate;
    this->d->m_blurFilter = AkElement::create("Blur");
    this->d->m_blurFilter->setProperty("radius", 2);

    QObject::connect(this->d->m_blurFilter.data(),
                     SIGNAL(radiusChanged(int)),
                     this,
                     SIGNAL(blurChanged(int)));
}

RadioactiveElement::~RadioactiveElement()
{
    delete this->d;
}

QString RadioactiveElement::mode() const
{
    return radiationModeToStr->value(this->d->m_mode);
}

int RadioactiveElement::blur() const
{
    return this->d->m_blurFilter->property("radius").toInt();
}

qreal RadioactiveElement::zoom() const
{
    return this->d->m_zoom;
}

int RadioactiveElement::threshold() const
{
    return this->d->m_threshold;
}

int RadioactiveElement::lumaThreshold() const
{
    return this->d->m_lumaThreshold;
}

int RadioactiveElement::alphaDiff() const
{
    return this->d->m_alphaDiff;
}

QRgb RadioactiveElement::radColor() const
{
    return this->d->m_radColor;
}

QImage RadioactiveElementPrivate::imageDiff(const QImage &img1,
                                            const QImage &img2,
                                            int threshold,
                                            int lumaThreshold,
                                            QRgb radColor,
                                            RadioactiveElement::RadiationMode mode)
{
    int width = qMin(img1.width(), img2.width());
    int height = qMin(img1.height(), img2.height());
    QImage diff(width, height, img1.format());

    for (int y = 0; y < height; y++) {
        const QRgb *iLine1 = reinterpret_cast<const QRgb *>(img1.constScanLine(y));
        const QRgb *iLine2 = reinterpret_cast<const QRgb *>(img2.constScanLine(y));
        QRgb *oLine = reinterpret_cast<QRgb *>(diff.scanLine(y));

        for (int x = 0; x < width; x++) {
            int r1 = qRed(iLine1[x]);
            int g1 = qGreen(iLine1[x]);
            int b1 = qBlue(iLine1[x]);

            int r2 = qRed(iLine2[x]);
            int g2 = qGreen(iLine2[x]);
            int b2 = qBlue(iLine2[x]);

            int dr = r1 - r2;
            int dg = g1 - g2;
            int db = b1 - b2;

            int alpha = dr * dr + dg * dg + db * db;
            alpha = int(sqrt(alpha / 3.0));

            if (mode == RadioactiveElement::RadiationModeSoftNormal
                || mode == RadioactiveElement::RadiationModeSoftColor)
                alpha = alpha < threshold? 0: alpha;
            else
                alpha = alpha < threshold? 0: 255;

            int gray = qGray(iLine2[x]);

            alpha = gray < lumaThreshold? 0: alpha;

            int r;
            int g;
            int b;

            if (mode == RadioactiveElement::RadiationModeHardNormal
                || mode == RadioactiveElement::RadiationModeSoftNormal) {
                r = r2;
                g = g2;
                b = b2;
            } else {
                r = qRed(radColor);
                g = qGreen(radColor);
                b = qBlue(radColor);
            }

            oLine[x] = qRgba(r, g, b, alpha);
        }
    }

    return diff;
}

QImage RadioactiveElementPrivate::imageAlphaDiff(const QImage &src,
                                                 int alphaDiff)
{
    QImage dest(src.size(), src.format());

    for (int y = 0; y < src.height(); y++) {
        const QRgb *srcLine = reinterpret_cast<const QRgb *>(src.constScanLine(y));
        QRgb *dstLine = reinterpret_cast<QRgb *>(dest.scanLine(y));

        for (int x = 0; x < src.width(); x++) {
            QRgb pixel = srcLine[x];
            int r = qRed(pixel);
            int g = qGreen(pixel);
            int b = qBlue(pixel);
            int a = qBound(0, qAlpha(pixel) + alphaDiff, 255);
            dstLine[x] = qRgba(r, g, b, a);
        }
    }

    return dest;
}

QString RadioactiveElement::controlInterfaceProvide(const QString &controlId) const
{
    Q_UNUSED(controlId)

    return QString("qrc:/Radioactive/share/qml/main.qml");
}

void RadioactiveElement::controlInterfaceConfigure(QQmlContext *context,
                                                   const QString &controlId) const
{
    Q_UNUSED(controlId)

    context->setContextProperty("Radioactive", const_cast<QObject *>(qobject_cast<const QObject *>(this)));
    context->setContextProperty("controlId", this->objectName());
}

void RadioactiveElement::setMode(const QString &mode)
{
    RadiationMode modeEnum = radiationModeToStr->key(mode,
                                                     RadiationModeSoftNormal);

    if (this->d->m_mode == modeEnum)
        return;

    this->d->m_mode = modeEnum;
    emit this->modeChanged(mode);
}

void RadioactiveElement::setBlur(int blur)
{
    this->d->m_blurFilter->setProperty("radius", blur);
}

void RadioactiveElement::setZoom(qreal zoom)
{
    if (qFuzzyCompare(this->d->m_zoom, zoom))
        return;

    this->d->m_zoom = zoom;
    emit this->zoomChanged(zoom);
}

void RadioactiveElement::setThreshold(int threshold)
{
    if (this->d->m_threshold == threshold)
        return;

    this->d->m_threshold = threshold;
    emit this->thresholdChanged(threshold);
}

void RadioactiveElement::setLumaThreshold(int lumaThreshold)
{
    if (this->d->m_lumaThreshold == lumaThreshold)
        return;

    this->d->m_lumaThreshold = lumaThreshold;
    emit this->lumaThresholdChanged(lumaThreshold);
}

void RadioactiveElement::setAlphaDiff(int alphaDiff)
{
    if (this->d->m_alphaDiff == alphaDiff)
        return;

    this->d->m_alphaDiff = alphaDiff;
    emit this->alphaDiffChanged(alphaDiff);
}

void RadioactiveElement::setRadColor(QRgb radColor)
{
    if (this->d->m_radColor == radColor)
        return;

    this->d->m_radColor = radColor;
    emit this->radColorChanged(radColor);
}

void RadioactiveElement::resetMode()
{
    this->setMode("softNormal");
}

void RadioactiveElement::resetBlur()
{
    this->setBlur(2);
}

void RadioactiveElement::resetZoom()
{
    this->setZoom(1.1);
}

void RadioactiveElement::resetThreshold()
{
    this->setThreshold(31);
}

void RadioactiveElement::resetLumaThreshold()
{
    this->setLumaThreshold(95);
}

void RadioactiveElement::resetAlphaDiff()
{
    this->setAlphaDiff(-8);
}

void RadioactiveElement::resetRadColor()
{
    this->setRadColor(qRgb(0, 255, 0));
}

AkPacket RadioactiveElement::iStream(const AkPacket &packet)
{
    AkVideoPacket videoPacket(packet);
    auto src = videoPacket.toImage();

    if (src.isNull())
        return AkPacket();

    src = src.convertToFormat(QImage::Format_ARGB32);
    QImage oFrame(src.size(), src.format());

    if (src.size() != this->d->m_frameSize) {
        this->d->m_blurZoomBuffer = QImage();
        this->d->m_prevFrame = QImage();
        this->d->m_frameSize = src.size();
    }

    if (this->d->m_prevFrame.isNull()) {
        oFrame = src;
        this->d->m_blurZoomBuffer = QImage(src.size(), src.format());
        this->d->m_blurZoomBuffer.fill(qRgba(0, 0, 0, 0));
    } else {
        // Compute the difference between previous and current frame,
        // and save it to the buffer.
        QImage diff =
                this->d->imageDiff(this->d->m_prevFrame,
                                   src,
                                   this->d->m_threshold,
                                   this->d->m_lumaThreshold,
                                   this->d->m_radColor,
                                   this->d->m_mode);

        QPainter painter;
        painter.begin(&this->d->m_blurZoomBuffer);
        painter.drawImage(0, 0, diff);
        painter.end();

        // Blur buffer.
        auto blurZoomPacket = AkVideoPacket::fromImage(this->d->m_blurZoomBuffer,
                                                       videoPacket);
        auto blurPacket = this->d->m_blurFilter->iStream(blurZoomPacket.toPacket());
        auto blur = AkVideoPacket(blurPacket).toImage();

        // Zoom buffer.
        QImage blurScaled = blur.scaled(this->d->m_zoom * blur.size());
        QSize diffSize = blur.size() - blurScaled.size();
        QPoint p(diffSize.width() >> 1,
                 diffSize.height() >> 1);

        QImage zoom(blur.size(), blur.format());
        zoom.fill(qRgba(0, 0, 0, 0));

        painter.begin(&zoom);
        painter.drawImage(p, blurScaled);
        painter.end();

        // Reduce alpha.
        QImage alphaDiff = this->d->imageAlphaDiff(zoom, this->d->m_alphaDiff);
        this->d->m_blurZoomBuffer = alphaDiff;

        // Apply buffer.
        painter.begin(&oFrame);
        painter.drawImage(0, 0, src);
        painter.drawImage(0, 0, this->d->m_blurZoomBuffer);
        painter.end();
    }

    this->d->m_prevFrame = src.copy();

    auto oPacket = AkVideoPacket::fromImage(oFrame, videoPacket).toPacket();
    akSend(oPacket)
}

#include "moc_radioactiveelement.cpp"
