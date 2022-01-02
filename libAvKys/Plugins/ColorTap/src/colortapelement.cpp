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

#include <QMutex>
#include <QImage>
#include <QQmlContext>
#include <QStandardPaths>
#include <akpacket.h>
#include <akvideopacket.h>

#include "colortapelement.h"

class ColorTapElementPrivate
{
    public:
        QImage m_table;
        QString m_tableName;
        QMutex m_mutex;
};

ColorTapElement::ColorTapElement(): AkElement()
{
    this->d = new ColorTapElementPrivate;
    this->d->m_tableName = ":/ColorTap/share/tables/base.bmp";
    this->d->m_table = QImage(this->d->m_tableName).scaled(16, 16);
}

ColorTapElement::~ColorTapElement()
{
    delete this->d;
}

QString ColorTapElement::table() const
{
    return this->d->m_tableName;
}

QString ColorTapElement::controlInterfaceProvide(const QString &controlId) const
{
    Q_UNUSED(controlId)

    return QString("qrc:/ColorTap/share/qml/main.qml");
}

void ColorTapElement::controlInterfaceConfigure(QQmlContext *context,
                                                const QString &controlId) const
{
    Q_UNUSED(controlId)

    context->setContextProperty("ColorTap", const_cast<QObject *>(qobject_cast<const QObject *>(this)));
    context->setContextProperty("controlId", this->objectName());

    QStringList picturesPath = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
    context->setContextProperty("picturesPath", picturesPath[0]);
}

AkPacket ColorTapElement::iVideoStream(const AkVideoPacket &packet)
{
    this->d->m_mutex.lock();

    if (this->d->m_table.isNull()) {
        this->d->m_mutex.unlock();
        akSend(packet)
    }

    auto src = packet.toImage();

    if (src.isNull()) {
        this->d->m_mutex.unlock();

        return AkPacket();
    }

    src = src.convertToFormat(QImage::Format_ARGB32);
    QImage oFrame(src.size(), src.format());
    auto tableBits = reinterpret_cast<const QRgb *>(this->d->m_table.constBits());

    for (int y = 0; y < src.height(); y++) {
        auto srcLine = reinterpret_cast<const QRgb *>(src.constScanLine(y));
        auto dstLine = reinterpret_cast<QRgb *>(oFrame.scanLine(y));

        for (int x = 0; x < src.width(); x++) {
            int r = qRed(srcLine[x]);
            int g = qGreen(srcLine[x]);
            int b = qBlue(srcLine[x]);

            int ro = qRed(tableBits[r]);
            int go = qGreen(tableBits[g]);
            int bo = qBlue(tableBits[b]);

            dstLine[x] = qRgb(ro, go, bo);
        }
    }

    this->d->m_mutex.unlock();

    auto oPacket = AkVideoPacket::fromImage(oFrame, packet);
    akSend(oPacket)
}

void ColorTapElement::setTable(const QString &table)
{
    if (this->d->m_tableName == table)
        return;

    QString tableName;
    QImage tableImg;

    if (!table.isEmpty()) {
        tableImg = QImage(table);

        if (tableImg.isNull()) {
            if (this->d->m_tableName.isNull())
                return;
        } else {
            tableName = table;
            tableImg = tableImg.scaled(16, 16);
        }
    }

    this->d->m_tableName = tableName;
    this->d->m_mutex.lock();
    this->d->m_table = tableImg;
    this->d->m_mutex.unlock();
    emit this->tableChanged(this->d->m_tableName);
}

void ColorTapElement::resetTable()
{
    this->setTable(":/ColorTap/share/tables/base.bmp");
}

#include "moc_colortapelement.cpp"
