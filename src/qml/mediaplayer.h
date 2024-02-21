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
#include <QQuickItem>

#include "audiooutput.h"
#include "core/instance.h"
#include "core/media.h"
#include "core/mediaplayer.h"
#include "mediasource.h"
#include "qml_shared_export.h"

class QUICKVLC_QML_EXPORT MediaPlayer : public MediaSource
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(AudioOutput *audioOutput READ audioOutput WRITE setAudioOutput NOTIFY audioOutputChanged)
    Q_PROPERTY(bool autoplay READ autoplay WRITE setAutoplay NOTIFY autoplayChanged)
    //    Q_PROPERTY(int activeAudioTrack)
    //    Q_PROPERTY(int activeSubtitleTrack)
    //    Q_PROPERTY(int activeVideoTrack)
    //    Q_PROPERTY(float bufferProgress)
    Q_PROPERTY(qint64 duration READ duration NOTIFY durationChanged)
    Q_PROPERTY(qint64 mediaDuration READ mediaDuration NOTIFY mediaDurationChanged)
    //    Q_PROPERTY(QString error)
    //    Q_PROPERTY(QString errorString)
    //    Q_PROPERTY(bool hasAudio)
    //    Q_PROPERTY(bool hasVideo)
    //    Q_PROPERTY(int loops)
    //    Q_PROPERTY(int mediaStatus)
    Q_PROPERTY(qint64 position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(bool seekable READ seekable NOTIFY seekableChanged)
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(int playbackState READ playbackState NOTIFY playbackStateChanged)
    Q_PROPERTY(QSize videoResolution READ videoResolution NOTIFY videoResolutionChanged)

public:
    explicit MediaPlayer(QQuickItem *parent = nullptr);

    ~MediaPlayer();

    Q_INVOKABLE void play();
    Q_INVOKABLE void pause();
    Q_INVOKABLE void stop();

    AudioOutput *audioOutput() const;
    void setAudioOutput(AudioOutput *audioOutput);

    bool autoplay() const;
    void setAutoplay(bool autoplay);

    QUrl source() const;
    void setSource(const QUrl &url);

    qint64 duration() const;

    qint64 mediaDuration() const;

    qint64 position() const;
    void setPosition(qint64 position);

    bool seekable() const;

    int playbackState() const;

    QSize videoResolution() const;

signals:
    void audioOutputChanged();
    void autoplayChanged();
    void durationChanged();
    void mediaDurationChanged();
    void positionChanged();
    void seekableChanged();
    void sourceChanged();
    void errorOccurred(QString errorString);
    void playbackStateChanged();
    void videoResolutionChanged();

private:
    void open();

    Vlc::Instance *m_instance;
    Vlc::Media *m_media;
    Vlc::MediaPlayer *m_player;

    AudioOutput *m_audioOutput;

    bool m_autoplay;

    Q_ENUM(Vlc::Enum::PlaybackState)
};
