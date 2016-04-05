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

#include <QtMath>

#include "photocopyelement.h"

PhotocopyElement::PhotocopyElement(): AkElement()
{
    this->m_brightness = 0.75;
    this->m_contrast = 20;
}

QObject *PhotocopyElement::controlInterface(QQmlEngine *engine,
                                            const QString &controlId) const
{
    Q_UNUSED(controlId)

    if (!engine)
        return NULL;

    // Load the UI from the plugin.
    QQmlComponent component(engine,
                            QUrl(QStringLiteral("qrc:/Photocopy/share/qml/main.qml")));

    if (component.isError()) {
        qDebug() << "Error in plugin "
                 << this->metaObject()->className()
                 << ":"
                 << component.errorString();

        return NULL;
    }

    // Create a context for the plugin.
    QQmlContext *context = new QQmlContext(engine->rootContext());
    context->setContextProperty("Photocopy", (QObject *) this);
    context->setContextProperty("controlId", this->objectName());

    // Create an item with the plugin context.
    QObject *item = component.create(context);

    if (!item) {
        delete context;

        return NULL;
    }

    context->setParent(item);

    return item;
}

qreal PhotocopyElement::brightness() const
{
    return this->m_brightness;
}

qreal PhotocopyElement::contrast() const
{
    return this->m_contrast;
}

void PhotocopyElement::setBrightness(qreal brightness)
{
    if (this->m_brightness == brightness)
        return;

    this->m_brightness = brightness;
    emit this->brightnessChanged(brightness);
}

void PhotocopyElement::setContrast(qreal contrast)
{
    if (this->m_contrast == contrast)
        return;

    this->m_contrast = contrast;
    emit this->contrastChanged(contrast);
}

void PhotocopyElement::resetBrightness()
{
    this->setBrightness(0.75);
}

void PhotocopyElement::resetContrast()
{
    this->setContrast(20);
}

AkPacket PhotocopyElement::iStream(const AkPacket &packet)
{
    QImage src = AkUtils::packetToImage(packet);

    if (src.isNull())
        return AkPacket();

    src = src.convertToFormat(QImage::Format_ARGB32);
    int videoArea = src.width() * src.height();
    QImage oFrame(src.size(), src.format());

    QRgb *srcBits = (QRgb *) src.bits();
    QRgb *destBits = (QRgb *) oFrame.bits();

    for (int i = 0; i < videoArea; i++) {
        int r = qRed(srcBits[i]);
        int g = qGreen(srcBits[i]);
        int b = qBlue(srcBits[i]);

        //desaturate
        int luma = this->rgbToLuma(r, g, b);

        //compute sigmoidal transfer
        qreal val = luma / 255.0;
        val = 255.0 / (1 + exp(this->m_contrast * (0.5 - val)));
        val = val * this->m_brightness;
        luma = qBound(0.0, val, 255.0);

        destBits[i] = qRgba(luma, luma, luma, qAlpha(srcBits[i]));
    }

    AkPacket oPacket = AkUtils::imageToPacket(oFrame, packet);
    akSend(oPacket)
}
