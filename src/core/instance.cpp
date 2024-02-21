/******************************************************************************
 * This file is part of QuickVLC - Qt and libvlc connection library.
 * Copyright (C) 2016 Tadej Novak <tadej@tano.si>,
 *               2022 Alexey Avramchik (OU Bamboo group) <aa@bam-boo.eu>
 *
 * QuickVLC is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published
 * by the Free Software Foundation, either version 3 of the License,
 * or (at your option) any later version.
 *
 * QuickVLC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with QuickVLC. If not, see <https://www.gnu.org/licenses/>.
 ******************************************************************************/

#include "instance.h"

#include <vlc/vlc.h>

#include "config.h"
#include "error.h"

#ifdef Q_OS_WIN
#include "compat/asprintf.h"
#endif

namespace Vlc {

void logCallback(void *data, int level, const libvlc_log_t *ctx, const char *fmt, va_list args)
{
    Q_UNUSED(ctx)

    auto *instance = static_cast<Instance *>(data);

    if (instance->logLevel() > level) {
        return;
    }

    char *result;

    if (vasprintf(&result, fmt, args) < 0) {
        return;  // LCOV_EXCL_LINE
    }

    QString message(result);
    free(result);

    message.prepend("libvlc: ");

    switch (level) {
    case Enum::ErrorLevel:
        qCritical(message.toUtf8().data(), NULL);
        break;
    case Enum::WarningLevel:
        qWarning(message.toUtf8().data(), NULL);
        break;
    case Enum::NoticeLevel:
    case Enum::DebugLevel:
    default:
        qDebug(message.toUtf8().data(), NULL);
        break;
    }
}

Instance::Instance(const QStringList &args, QObject *parent)
    : QObject(parent), m_vlcInstance { nullptr }, m_status { false }, m_logLevel { Enum::ErrorLevel }
{
    char **argv = (char **)malloc(sizeof(char *) * args.count());

    for (int i = 0; i < args.count(); ++i) {
        argv[i] = (char *)qstrdup(args.at(i).toUtf8().data());
    }

    m_vlcInstance = libvlc_new(args.count(), argv);

    Error::printErrorMsg();

    if (m_vlcInstance) {
        libvlc_log_set(m_vlcInstance, logCallback, this);

        m_status = true;

        qDebug() << "QuickVLC" << libVersion() << "initialized";
        qDebug() << "Using libvlc version:" << version();
    } else {
        qCritical() << "libvlc failed to load!";
    }

    for (int i = 0; i < args.count(); ++i) {
        free(argv[i]);
    }
    free(argv);
}

Instance::~Instance()
{
    if (m_status && m_vlcInstance) {
        libvlc_release(m_vlcInstance);
    }
}

libvlc_instance_t *Instance::core()
{
    return m_vlcInstance;
}

bool Instance::status() const
{
    return m_status;
}

QString Instance::libVertion()
{
    return {};
}

QString Instance::version()
{
    return QString { libvlc_get_version() };
}

QString Instance::libVersion()
{
    QString version;

#if defined(QUICKVLC_VERSION)
    version.append(QString { QUICKVLC_VERSION });
#else
    version.append(QString { "Unknown" });
#endif

#if defined(QUICKVLC_REVISION)
    if (QUICKVLC_REVISION != 0 && !QString(QUICKVLC_REVISION).isEmpty()) {
        version.append("-" + QString { QUICKVLC_REVISION });
    }
#endif

    return version;
}

Enum::LogLevel Instance::logLevel() const
{
    return m_logLevel;
}

void Instance::setLogLevel(Enum::LogLevel level)
{
    m_logLevel = level;
}

}  // namespace Vlc
