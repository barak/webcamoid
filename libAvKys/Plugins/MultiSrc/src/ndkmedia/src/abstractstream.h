/* Webcamoid, webcam capture application.
 * Copyright (C) 2019  Gonzalo Exequiel Pedone
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

#ifndef ABSTRACTSTREAM_H
#define ABSTRACTSTREAM_H

#include <akelement.h>

class AbstractStream;
class AbstractStreamPrivate;
class AkFrac;
class AkCaps;
class AkPacket;
class Clock;
struct AMediaExtractor;
struct AMediaFormat;
struct AMediaCodec;
using AbstractStreamPtr = QSharedPointer<AbstractStream>;

class AbstractStream: public QObject
{
    Q_OBJECT

    public:
        AbstractStream(AMediaExtractor *mediaExtractor=nullptr,
                       uint index=0,
                       qint64 id=-1,
                       Clock *globalClock=nullptr,
                       bool sync=true,
                       QObject *parent=nullptr);
        virtual ~AbstractStream();

        Q_INVOKABLE bool isValid() const;
        Q_INVOKABLE uint index() const;
        Q_INVOKABLE qint64 id() const;
        Q_INVOKABLE AkFrac timeBase() const;
        Q_INVOKABLE QString mimeType() const;
        Q_INVOKABLE AMediaCodec *codec() const;
        Q_INVOKABLE AMediaFormat *mediaFormat() const;
        Q_INVOKABLE virtual AkCaps caps() const;
        Q_INVOKABLE bool sync() const;
        Q_INVOKABLE Clock *globalClock();
        Q_INVOKABLE qreal clockDiff() const;
        Q_INVOKABLE qreal &clockDiff();
        Q_INVOKABLE bool packetEnqueue(bool eos=false);
        Q_INVOKABLE void dataEnqueue(const AkPacket &packet);
        Q_INVOKABLE virtual bool decodeData();
        Q_INVOKABLE static QString mimeType(AMediaExtractor *mediaExtractor,
                                            uint index);
        Q_INVOKABLE AkElement::ElementState state() const;

    protected:
        bool m_isValid;
        qreal m_clockDiff;
        int m_maxData;

        virtual void processData(const AkPacket &packet);

    private:
        AbstractStreamPrivate *d;

    signals:
        void stateChanged(AkElement::ElementState state);
        void oStream(const AkPacket &packet);
        void eof();

    public slots:
        void flush();
        bool setState(AkElement::ElementState state);

        friend class AbstractStreamPrivate;
};

#endif // ABSTRACTSTREAM_H
