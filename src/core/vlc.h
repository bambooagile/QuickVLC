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

#pragma once

#include <QObject>
#include <QSizeF>
#include <QStringList>

#include "core_shared_export.h"

namespace Vlc {

class QUICKVLC_CORE_EXPORT Enum : public QObject
{
    Q_OBJECT

public:
    explicit Enum(QObject *parent = nullptr);

    enum LogLevel { DebugLevel = 0, NoticeLevel = 2, WarningLevel = 3, ErrorLevel = 4, DisabledLevel = 5 };

    enum RenderFormat { YUVFormat };

    enum ActionsType { AudioTrack, Subtitles, VideoTrack, Other };

    enum AudioChannel { AudioChannelError = -1, Stereo = 1, RStereo = 2, Left = 3, Right = 4, Dolbys = 5 };

    enum AudioCodec { NoAudio, MPEG2Audio, MP3, MPEG4Audio, Vorbis, Flac };

    enum AudioOutput { DefaultAout };

    enum Deinterlacing { Disabled, Discard, Blend, Mean, Bob, Linear, X, Yadif, Yadif2x, Phospor, IVTC };

    enum FillMode {
        PreserveAspectFit = Qt::KeepAspectRatio,
        PreserveAspectCrop = Qt::KeepAspectRatioByExpanding,
        Stretch = Qt::IgnoreAspectRatio
    };

    enum Meta {
        Title,
        Artist,
        Genre,
        Copyright,
        Album,
        TrackNumber,
        Description,
        Rating,
        Date,
        Setting,
        URL,
        Language,
        NowPlaying,
        Publisher,
        EncodedBy,
        ArtworkURL,
        TrackID
    };

    enum Mux { TS, PS, MP4, OGG, AVI };

    enum PlaybackMode { DefaultPlayback, Loop, Repeat };

    enum Ratio {
        Original,
        Ignore, /*!< QML aspect ratio only */
        R_16_9,
        R_16_10,
        R_185_100,
        R_221_100,
        R_235_100,
        R_239_100,
        R_4_3,
        R_5_4,
        R_5_3,
        R_1_1
    };

    enum Scale { NoScale, S_1_05, S_1_1, S_1_2, S_1_3, S_1_4, S_1_5, S_1_6, S_1_7, S_1_8, S_1_9, S_2_0 };

    enum PlaybackState { Idle, Opening, Playing, Paused, Stopped, Stopping, Error };

    enum VideoCodec { NoVideo, MPEG2Video, MPEG4Video, H264, Theora };

    enum VideoOutput {
#if defined(Q_OS_LINUX)
        X11,
        XVideo,
        GLX,
#elif defined(Q_OS_WIN32)
        DirectX,
        Direct3D,
#endif
        OpenGL,
        DefaultVout
    };

    static QStringList logLevel();
    static QStringList audioCodec();
    static QStringList audioOutput();
    static QStringList audioOutputHuman();
    static QStringList deinterlacing();
    static QStringList mux();
    static QStringList ratio();
    static QStringList ratioHuman();
    static QSizeF ratioSize(const Enum::Ratio &ratio);
    static QList<float> scale();
    static QStringList videoCodec();
    static QStringList videoOutput();
    static QStringList videoOutputHuman();

private:
    Q_ENUM(LogLevel)
    Q_ENUM(ActionsType)
    Q_ENUM(AudioChannel)
    Q_ENUM(AudioCodec)
    Q_ENUM(AudioOutput)
    Q_ENUM(Deinterlacing)
    Q_ENUM(FillMode)
    Q_ENUM(Meta)
    Q_ENUM(Mux)
    Q_ENUM(PlaybackMode)
    Q_ENUM(Ratio)
    Q_ENUM(Scale)
    Q_ENUM(PlaybackState)
    Q_ENUM(VideoCodec)
    Q_ENUM(VideoOutput)
};

}  // namespace Vlc
