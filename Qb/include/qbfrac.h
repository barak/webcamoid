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

#ifndef QBFRAC_H
#define QBFRAC_H

#include <QObject>
#include <QDebug>

class QbFracPrivate;

class QbFrac: public QObject
{
    Q_OBJECT
    Q_PROPERTY(int num
               READ num
               WRITE setNum
               RESET resetNum
               NOTIFY numChanged)
    Q_PROPERTY(int den
               READ den
               WRITE setDen
               RESET resetDen
               NOTIFY denChanged)
    Q_PROPERTY(bool isValid
               READ isValid
               NOTIFY isValidChanged)
    Q_PROPERTY(double value
               READ value
               NOTIFY valueChanged)
    Q_PROPERTY(QString string
               READ toString
               NOTIFY stringChanged)

    public:
        explicit QbFrac(QObject *parent=NULL);
        QbFrac(qint64 num, qint64 den);
        QbFrac(const QString &fracString);
        QbFrac(const QbFrac &other);
        virtual ~QbFrac();
        QbFrac &operator =(const QbFrac &other);
        bool operator ==(const QbFrac &other) const;
        bool operator !=(const QbFrac &other) const;
        QbFrac operator *(const QbFrac &other) const;

        Q_INVOKABLE qint64 num() const;
        Q_INVOKABLE qint64 den() const;
        Q_INVOKABLE double value() const;
        Q_INVOKABLE int fastValue() const;
        Q_INVOKABLE bool isValid() const;
        Q_INVOKABLE QString toString() const;
        Q_INVOKABLE QbFrac invert() const;

    private:
        QbFracPrivate *d;

    signals:
        void numChanged();
        void denChanged();
        void isValidChanged();
        void valueChanged();
        void stringChanged();

    public slots:
        void setNumDen(qint64 num, qint64 den);
        void setNumDen(const QString &fracString);
        void setNum(qint64 num);
        void setDen(qint64 den);
        void resetNum();
        void resetDen();

    friend QDebug operator <<(QDebug debug, const QbFrac &frac);
    friend QDataStream &operator >>(QDataStream &istream, QbFrac &frac);
    friend QDataStream &operator <<(QDataStream &ostream, const QbFrac &frac);
    friend QbFrac operator *(int number, const QbFrac &frac);
    friend QbFrac operator /(const QbFrac &fracNum, const QbFrac &fracDen);
};

QDebug operator <<(QDebug debug, const QbFrac &frac);
QDataStream &operator >>(QDataStream &istream, QbFrac &frac);
QDataStream &operator <<(QDataStream &ostream, const QbFrac &frac);
QbFrac operator *(int number, const QbFrac &frac);
QbFrac operator /(int number, const QbFrac &frac);
QbFrac operator /(const QbFrac &fracNum, const QbFrac &fracDen);

Q_DECLARE_METATYPE(QbFrac)

#endif // QBFRAC_H
