/* Webcamoid, webcam capture application.
 * Copyright (C) 2017  Gonzalo Exequiel Pedone
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

#include <QTimer>
#include <QDateTime>
#include <QSettings>
#include <QtQml>
#include <QQmlContext>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#include "updates.h"

#define UPDATES_URL "https://api.github.com/repos/webcamoid/webcamoid/releases/latest"

class UpdatesPrivate
{
    public:
        QQmlApplicationEngine *m_engine {nullptr};
        QNetworkAccessManager m_manager;
        QString m_latestVersion {COMMONS_VERSION};
        QDateTime m_lastUpdate;
        QTimer m_timer;
        Updates::VersionType m_versionType {Updates::VersionTypeCurrent};
        int m_checkInterval {0};
        bool m_notifyNewVersion {false};

        QVariantList vectorize(const QString &version) const;
        void normalize(QVariantList &vector1, QVariantList &vector2) const;
        template<typename Functor>
        bool compare(const QString &version1,
                     const QString &version2,
                     Functor func) const {
            auto v1 = this->vectorize(version1);
            auto v2 = this->vectorize(version2);
            this->normalize(v1, v2);
            QString sv1;
            QString sv2;

            for (int i = 0; i < v1.size(); i++) {
                auto fillChar = v1[i].type() == QVariant::String? ' ': '0';
                auto a = v1[i].toString();
                auto b = v2[i].toString();
                auto width = qMax(a.size(), b.size());

                sv1 += QString("%1").arg(a, width, fillChar);
                sv2 += QString("%1").arg(b, width, fillChar);
            }

            return func(sv1, sv2);
        }
};

Updates::Updates(QQmlApplicationEngine *engine, QObject *parent):
    QObject(parent)
{
    this->d = new UpdatesPrivate;
    this->setQmlEngine(engine);

    QObject::connect(&this->d->m_manager,
                     &QNetworkAccessManager::finished,
                     this,
                     &Updates::replyFinished);

    // Check lasUpdate every 10 mins
    this->d->m_timer.setInterval(int(1e3 * 60 * 10));
    this->loadProperties();

    QObject::connect(&this->d->m_timer,
                     &QTimer::timeout,
                     this,
                     &Updates::checkUpdates);
    QObject::connect(this,
                     &Updates::notifyNewVersionChanged,
                     this,
                     &Updates::saveNotifyNewVersion);
    QObject::connect(this,
                     &Updates::latestVersionChanged,
                     this,
                     &Updates::saveLatestVersion);
    QObject::connect(this,
                     &Updates::checkIntervalChanged,
                     this,
                     &Updates::saveCheckInterval);
    QObject::connect(this,
                     &Updates::checkIntervalChanged,
                     [this] (int checkInterval) {
                        if (checkInterval > 0)
                            this->d->m_timer.start();
                        else
                            this->d->m_timer.stop();
                     });
    QObject::connect(this,
                     &Updates::lastUpdateChanged,
                     this,
                     &Updates::saveLastUpdate);
}

Updates::~Updates()
{
    this->saveProperties();
    delete this->d;
}

bool Updates::notifyNewVersion() const
{
    return this->d->m_notifyNewVersion;
}

Updates::VersionType Updates::versionType() const
{
    return this->d->m_versionType;
}

QString Updates::latestVersion() const
{
    return this->d->m_latestVersion;
}

int Updates::checkInterval() const
{
    return this->d->m_checkInterval;
}

QDateTime Updates::lastUpdate() const
{
    if (this->d->m_lastUpdate.isValid())
        return this->d->m_lastUpdate;

    return QDateTime::currentDateTime();
}

QVariantList UpdatesPrivate::vectorize(const QString &version) const
{
    QVariantList vector;
    QString digs;
    QString alps;

    for (auto &c: version) {
        if (c.isDigit()) {
            digs += c;

            if (!alps.isEmpty()) {
                vector << alps;
                alps.clear();
            }
        } else if (c.isLetter()) {
            alps += c;

            if (!digs.isEmpty()) {
                vector << digs.toUInt();
                digs.clear();
            }
        } else {
            if (!digs.isEmpty()) {
                vector << digs.toUInt();
                digs.clear();
            }

            if (!alps.isEmpty()) {
                vector << alps;
                alps.clear();
            }
        }
    }

    if (!digs.isEmpty()) {
        vector << digs.toUInt();
        digs.clear();
    }

    if (!alps.isEmpty()) {
        vector << alps;
        alps.clear();
    }

    return vector;
}

void UpdatesPrivate::normalize(QVariantList &vector1,
                               QVariantList &vector2) const
{
    auto diff = vector1.size() - vector2.size();

    if (diff > 0) {
        auto offset = vector2.size();

        for (int i = 0; i < diff; i++)
            if (vector1.value(offset + i).type() == QVariant::String)
                vector2 << QString();
            else
                vector2 << 0;
    } else if (diff < 0) {
        diff *= -1;
        auto offset = vector1.size();

        for (int i = 0; i < diff; i++)
            if (vector2.value(offset + i).type() == QVariant::String)
                vector1 << QString();
            else
                vector1 << 0;
    }
}

void Updates::checkUpdates()
{
    if (this->d->m_checkInterval > 0
        &&(this->d->m_lastUpdate.isNull()
           || this->d->m_lastUpdate.daysTo(QDateTime::currentDateTime()) >= this->d->m_checkInterval)) {
        this->d->m_manager.get(QNetworkRequest(QUrl(UPDATES_URL)));
    }
}

void Updates::setQmlEngine(QQmlApplicationEngine *engine)
{
    if (this->d->m_engine == engine)
        return;

    this->d->m_engine = engine;

    if (engine) {
        engine->rootContext()->setContextProperty("Updates", this);
        qmlRegisterType<Updates>("WebcamoidUpdates", 1, 0, "UpdatesT");
    }
}

void Updates::setNotifyNewVersion(bool notifyNewVersion)
{
    if (this->d->m_notifyNewVersion == notifyNewVersion)
        return;

    this->d->m_notifyNewVersion = notifyNewVersion;
    emit this->notifyNewVersionChanged(notifyNewVersion);
}

void Updates::setCheckInterval(int checkInterval)
{
    if (this->d->m_checkInterval == checkInterval)
        return;

    this->d->m_checkInterval = checkInterval;
    emit this->checkIntervalChanged(checkInterval);
}

void Updates::resetNotifyNewVersion()
{
    this->setNotifyNewVersion(false);
}

void Updates::resetCheckInterval()
{
    this->setCheckInterval(0);
}

void Updates::setVersionType(Updates::VersionType versionType)
{
    if (this->d->m_versionType == versionType)
        return;

    this->d->m_versionType = versionType;
    emit this->versionTypeChanged(versionType);
}

void Updates::setLatestVersion(const QString &latestVersion)
{
    if (this->d->m_latestVersion == latestVersion)
        return;

    this->d->m_latestVersion = latestVersion;
    emit this->latestVersionChanged(latestVersion);
}

void Updates::setLastUpdate(const QDateTime &lastUpdate)
{
    if (this->d->m_lastUpdate == lastUpdate)
        return;

    this->d->m_lastUpdate = lastUpdate;
    emit this->lastUpdateChanged(lastUpdate);
}

void Updates::replyFinished(QNetworkReply *reply)
{
    if (!reply || reply->error() != QNetworkReply::NoError) {
        if (!reply)
            qDebug() << "Error requesting latest version:" << reply->errorString();
        else
            qDebug() << "Error requesting latest version: No response";

        return;
    }

    QString html = reply->readAll();
    QJsonParseError error {0, QJsonParseError::NoError};
    auto json = QJsonDocument::fromJson(html.toUtf8(), &error);

    if (error.error != QJsonParseError::NoError) {
        qDebug() << "Error requesting latest version:"
                 << error.errorString();

        return;
    }

    if (!json.isObject())
        return;

    auto jsonObj = json.object();

    if (!jsonObj.contains("tag_name"))
        return;

    auto version = jsonObj.value("tag_name").toString(COMMONS_VERSION);
    this->setLatestVersion(version);

#ifdef DAILY_BUILD
    this->setVersionType(VersionTypeDevelopment);
#else
    auto isOldVersion =
            this->d->compare(version, COMMONS_VERSION,
                             [] (const QVariant &a, const QVariant &b) {
                                   return a > b;
                             });
    VersionType versionType = isOldVersion?
                                  VersionTypeOld:
                              version == COMMONS_VERSION?
                                   VersionTypeCurrent:
                                   VersionTypeDevelopment;
    this->setVersionType(versionType);
#endif
    this->setLastUpdate(QDateTime::currentDateTime());
}

void Updates::loadProperties()
{
    QSettings config;

    config.beginGroup("Updates");
    this->setLastUpdate(config.value("lastUpdate").toDateTime());
    this->setNotifyNewVersion(config.value("notify", true).toBool());
    this->setCheckInterval(config.value("checkInterval", 1).toInt());
    this->setLatestVersion(config.value("latestVersion", COMMONS_VERSION).toString());
    config.endGroup();

   if (this->d->m_checkInterval > 0) {
       this->d->m_timer.start();
       this->checkUpdates();
   } else
       this->d->m_timer.stop();

#ifdef DAILY_BUILD
   this->setVersionType(VersionTypeDevelopment);
#else
   auto isOldVersion =
           this->d->compare(this->d->m_latestVersion, COMMONS_VERSION,
                            [] (const QVariant &a, const QVariant &b) {
                                  return a > b;
                            });
   VersionType versionType = isOldVersion?
                                 VersionTypeOld:
                             this->d->m_latestVersion == COMMONS_VERSION?
                                  VersionTypeCurrent:
                                  VersionTypeDevelopment;

   this->setVersionType(versionType);
#endif
}

void Updates::saveNotifyNewVersion(bool notifyNewVersion)
{
    QSettings config;

    config.beginGroup("Updates");
    config.setValue("notify", notifyNewVersion);
    config.endGroup();
}

void Updates::saveLatestVersion(const QString &latestVersion)
{
    QSettings config;

    config.beginGroup("Updates");
    config.setValue("latestVersion", latestVersion);
    config.endGroup();
}

void Updates::saveCheckInterval(int checkInterval)
{
    QSettings config;

    config.beginGroup("Updates");
    config.setValue("checkInterval", checkInterval);
    config.endGroup();
}

void Updates::saveLastUpdate(const QDateTime &lastUpdate)
{
    QSettings config;

    config.beginGroup("Updates");
    config.setValue("lastUpdate", lastUpdate);
    config.endGroup();
}

void Updates::saveProperties()
{
    QSettings config;

    auto lastUpdate = this->d->m_lastUpdate;

    if (!lastUpdate.isValid())
        lastUpdate = QDateTime::currentDateTime();

    config.beginGroup("Updates");
    config.setValue("lastUpdate", lastUpdate);
    config.setValue("notify", this->d->m_notifyNewVersion);
    config.setValue("checkInterval", this->d->m_checkInterval);
    config.setValue("latestVersion", this->d->m_latestVersion);
    config.endGroup();
}

#include "moc_updates.cpp"
