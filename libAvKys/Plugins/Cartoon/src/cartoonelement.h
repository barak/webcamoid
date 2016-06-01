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
 * Web-Site: http://webcamoid.github.io/
 */

#ifndef CARTOONELEMENT_H
#define CARTOONELEMENT_H

#include <QQmlComponent>
#include <QQmlContext>
#include <ak.h>
#include <akutils.h>

#include "pixel.h"

class CartoonElement: public AkElement
{
    Q_OBJECT
    Q_PROPERTY(int threshold
               READ threshold
               WRITE setThreshold
               RESET resetThreshold
               NOTIFY thresholdChange)
    Q_PROPERTY(QSize scanSize
               READ scanSize
               WRITE setScanSize
               RESET resetScanSize
               NOTIFY scanSizeChanged)

    public:
        explicit CartoonElement();

        Q_INVOKABLE QObject *controlInterface(QQmlEngine *engine,
                                              const QString &controlId) const;

        Q_INVOKABLE int threshold() const;
        Q_INVOKABLE QSize scanSize() const;

    private:
        int m_threshold;
        QSize m_scanSize;
        QVector<PixelInt> m_palette;
        qint64 m_id;
        qint64 m_lastTime;

        QVector<QRgb> palette(const QImage &img);
        QRgb nearestColor(const QVector<QRgb> &palette, QRgb color) const;

    signals:
        void thresholdChange(int threshold);
        void scanSizeChanged(QSize scanSize);

    public slots:
        void setThreshold(int threshold);
        void setScanSize(QSize scanSize);
        void resetThreshold();
        void resetScanSize();
        AkPacket iStream(const AkPacket &packet);
};

#endif // CARTOONELEMENT_H
