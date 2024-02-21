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

#include "common.h"

namespace Vlc {
namespace Common {

QStringList args()
{
    QStringList argsList;

    QString args = qgetenv("VLC_ARGS");

    if (!args.isEmpty()) {
        argsList = args.split(" ", Qt::SkipEmptyParts);
    } else {
        argsList << "--intf=dummy"
                 << "--no-media-library"
                 << "--no-stats"
                 << "--no-osd"
                 << "--no-loop"
                 << "--no-video-title-show"
#if defined(Q_OS_WIN)
                 << "--dec-dev=dxva2"
#endif
#if defined(Q_OS_DARWIN)
                 << "--vout=macosx"
#endif
                 << "--drop-late-frames";
    }

   // argsList.append("-vvvv");
   // argsList.append("--dec-dev=none");

    return argsList;
}

bool setPluginPath(const QString &path)
{
    if (qEnvironmentVariableIsEmpty("VLC_PLUGIN_PATH")) {
        return qputenv("VLC_PLUGIN_PATH", path.toLocal8Bit());
    }

    return false;
}

}  // namespace Common
}  // namespace Vlc
