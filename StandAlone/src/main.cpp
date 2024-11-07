/* Webcamoid, webcam capture application.
 * Copyright (C) 2015  Gonzalo Exequiel Pedone
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

#include <QDebug>
#include <QApplication>
#include <QDirIterator>
#include <QFontDatabase>
#include <QMutex>
#include <QTranslator>

#include "clioptions.h"
#include "mediatools.h"

int main(int argc, char *argv[])
{
    QApplication::setApplicationName(COMMONS_APPNAME);
    QApplication::setApplicationVersion(COMMONS_VERSION);
    QApplication::setOrganizationName(COMMONS_APPNAME);
    QApplication::setOrganizationDomain(ORGANIZATION_DOMAIN);
    qInstallMessageHandler(MediaTools::messageHandler);

    QApplication app(argc, argv);
    CliOptions cliOptions;

    // Install translations.

    QTranslator translator;

#ifdef Q_OS_ANDROID
    auto dataRootDir = QDir(DATAROOTDIR).absolutePath();
    auto translationsDir = QDir(TRANSLATIONSDIR).absolutePath();
    auto relTranslationsDir = QDir(dataRootDir).relativeFilePath(translationsDir);

    if (translator.load(QLocale::system().name(),
                        QString("assets:/%1").arg(relTranslationsDir)))
        QCoreApplication::installTranslator(&translator);
#else
    auto binDir = QDir(BINDIR).absolutePath();
    auto translationsDir = QDir(TRANSLATIONSDIR).absolutePath();
    auto relTranslationsDir = QDir(binDir).relativeFilePath(translationsDir);
    QDir appDir = QCoreApplication::applicationDirPath();

    if (appDir.cd(relTranslationsDir)) {
        auto path = appDir.absolutePath();
        path.replace("/", QDir::separator());

        if (QFileInfo::exists(path)
            && translator.load(QLocale::system().name(), path)) {
            QCoreApplication::installTranslator(&translator);
        }
    }
#endif

    // Set fonts
    QDirIterator fontsDirIterator(":/Webcamoid/share/themes/WebcamoidTheme/fonts",
                                  QStringList() << "*.ttf",
                                  QDir::Files
                                  | QDir::Readable
                                  | QDir::NoDotAndDotDot,
                                  QDirIterator::Subdirectories);

    while (fontsDirIterator.hasNext())
        QFontDatabase::addApplicationFont(fontsDirIterator.next());

#if defined(Q_OS_WIN32) || defined(Q_OS_OSX)
    // NOTE: OpenGL detection in Qt is quite buggy, so use software render by default.
    auto quickBackend = qgetenv("QT_QUICK_BACKEND");

    if (quickBackend.isEmpty())
        qputenv("QT_QUICK_BACKEND", "software");
#elif defined(Q_OS_ANDROID)
    qputenv("ANDROID_OPENSSL_SUFFIX", ANDROID_OPENSSL_SUFFIX);

    // Enable for plugins debug

    #if defined(QT_DEBUG) || defined(ENABLE_ANDROID_DEBUGGING)
        qputenv("QT_DEBUG_PLUGINS", "1");
        qputenv("QT_LOGGING_RULES", "*.debug=true");
    #endif
#elif defined(Q_OS_UNIX)
    // NOTE: Text is not rendered with QQC2, use native rendering.
    auto distanceField = qgetenv("QML_DISABLE_DISTANCEFIELD");

    if (distanceField.isEmpty())
        qputenv("QML_DISABLE_DISTANCEFIELD", "1");
#endif

    qDebug() << "Starting " COMMONS_APPNAME;
    MediaTools mediaTools;

    if (!mediaTools.init(cliOptions))
        return -1;

    mediaTools.printLog();
    mediaTools.show();

    return app.exec();
}
