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

#include "binelement.h"

BinElement::BinElement(): QbElement()
{
    this->m_pipelineDescription.setParent(this);

    this->resetDescription();
    this->resetBlocking();
    this->resetThreads();
}

QString BinElement::description() const
{
    return this->m_description;
}

bool BinElement::blocking() const
{
    return this->m_blocking;
}

ThreadsMap BinElement::threads() const
{
    return this->m_threads;
}

QbElementPtr BinElement::element(const QString &elementName)
{
    return this->m_elements[elementName];
}

void BinElement::add(QbElementPtr element)
{
    this->m_pipelineDescription.addElement(element);
}

void BinElement::remove(const QString &elementName)
{
    this->m_pipelineDescription.removeElement(elementName);
}

void BinElement::setDescription(const QString &description)
{
    if (this->m_description == description)
        return;

    ElementState preState = this->state();

    this->setState(ElementStateNull);

    if (this->m_description.isEmpty())
    {
        this->m_pipelineDescription.parse(description);
        QString error = this->m_pipelineDescription.error();

        if (error.isEmpty()) {
            this->m_description = description;

            this->m_elements = this->m_pipelineDescription.elements();
            this->m_inputs = this->m_pipelineDescription.inputs();
            this->m_outputs = this->m_pipelineDescription.outputs();
            this->connectOutputs();
        } else {
            this->m_pipelineDescription.cleanAll();

            qDebug() << error;
        }
    } else if (description.isEmpty()) {
        this->m_pipelineDescription.cleanAll();
        this->m_description = description;
    } else {
        foreach (QbElementPtr element, this->m_outputs)
            QObject::disconnect(element.data(),
                                SIGNAL(oStream(const QbPacket &)),
                                this,
                                SIGNAL(oStream(const QbPacket &)));

        this->m_pipelineDescription.cleanAll();

        this->m_pipelineDescription.parse(description);
        QString error = this->m_pipelineDescription.error();

        if (error.isEmpty()) {
            this->m_description = description;

            this->m_elements = this->m_pipelineDescription.elements();
            this->m_inputs = this->m_pipelineDescription.inputs();
            this->m_outputs = this->m_pipelineDescription.outputs();
            this->connectOutputs();
        } else {
            this->m_pipelineDescription.cleanAll();
            this->m_description = "";

            qDebug() << error;
        }
    }

    this->setState(preState);
}

void BinElement::setBlocking(bool blocking)
{
    this->m_blocking = blocking;
}

void BinElement::setThreads(const ThreadsMap &threads)
{
    this->m_threads = threads;
    this->m_pipelineDescription.setThreads(threads);
}

void BinElement::resetDescription()
{
    this->setDescription("");
}

void BinElement::resetBlocking()
{
    this->setBlocking(false);
}

void BinElement::resetThreads()
{
    this->m_threads.clear();
    this->m_pipelineDescription.resetThreads();
}

QbPacket BinElement::iStream(const QbPacket &packet)
{
    if (!this->description().isEmpty())
        foreach (QbElementPtr element, this->m_inputs)
            element->iStream(packet);
    else if (!this->blocking())
        qbSend(packet)

    return QbPacket();
}

void BinElement::setState(QbElement::ElementState state)
{
    QbElement::setState(state);

    foreach (QbElementPtr element, this->m_elements)
        QMetaObject::invokeMethod(element.data(),
                                  "setState",
                                  Q_ARG(QbElement::ElementState,
                                        this->state()));
}

void BinElement::connectOutputs()
{
    QList<Qt::ConnectionType> connectionTypes = this->m_pipelineDescription.outputConnectionTypes();
    int i = 0;

    foreach (QbElementPtr element, this->m_outputs)
    {
        QObject::connect(element.data(),
                         SIGNAL(oStream(const QbPacket &)),
                         this,
                         SIGNAL(oStream(const QbPacket &)),
                         connectionTypes[i]);

        i++;
    }
}

void BinElement::disconnectOutputs()
{
    foreach (QbElementPtr element, this->m_outputs)
        QObject::disconnect(element.data(),
                            SIGNAL(oStream(const QbPacket &)),
                            this,
                            SIGNAL(oStream(const QbPacket &)));
}
