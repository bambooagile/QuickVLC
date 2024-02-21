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

#include "mediaplayer.h"

#include <vlc/vlc.h>

#include "error.h"
#include "instance.h"
#include "media.h"

namespace Vlc {

MediaPlayer::MediaPlayer(Instance *instance) : QObject { instance }
{
    m_vlcInstance = instance->core();
    m_vlcMediaPlayer = libvlc_media_player_new(instance->core());
    m_vlcEvents = libvlc_media_player_event_manager(m_vlcMediaPlayer);

    libvlc_video_set_key_input(m_vlcMediaPlayer, false);
    libvlc_video_set_mouse_input(m_vlcMediaPlayer, false);

    Error::printErrorMsg();

    m_media = nullptr;

    createCoreConnections();

    connect(this, &MediaPlayer::nothingSpecial, this, &MediaPlayer::playbackStateChanged);
    connect(this, &MediaPlayer::opening, this, &MediaPlayer::playbackStateChanged);
    connect(this, &MediaPlayer::paused, this, &MediaPlayer::playbackStateChanged);
    connect(this, &MediaPlayer::playing, this, &MediaPlayer::playbackStateChanged);
    connect(this, &MediaPlayer::error, this, &MediaPlayer::playbackStateChanged);
    connect(this, &MediaPlayer::stopping, this, &MediaPlayer::playbackStateChanged);
    connect(this, &MediaPlayer::stopped, this, &MediaPlayer::playbackStateChanged);

    Error::printErrorMsg();
}

MediaPlayer::~MediaPlayer()
{
    removeCoreConnections();

    libvlc_media_player_release(m_vlcMediaPlayer);

    Error::printErrorMsg();
}

libvlc_media_player_t *MediaPlayer::core() const
{
    return m_vlcMediaPlayer;
}

libvlc_event_manager_t *MediaPlayer::eventManager() const
{
    return m_vlcEvents;
}

int MediaPlayer::length() const
{
    return m_length;
}

libvlc_media_t *MediaPlayer::currentMediaCore()
{
    auto *media = libvlc_media_player_get_media(m_vlcMediaPlayer);

    Error::printErrorMsg();

    return media;
}

void MediaPlayer::open(Media *media)
{
    openOnly(media);

    play();
}

void MediaPlayer::openOnly(Media *media)
{
    m_media = media;

    libvlc_media_player_set_media(m_vlcMediaPlayer, media->core());
    
    Error::printErrorMsg();
}

int MediaPlayer::time() const
{
    return m_time;
}

bool MediaPlayer::seekable() const
{
    if (!libvlc_media_player_get_media(m_vlcMediaPlayer)) {
        return false;
    }

    bool seekable = libvlc_media_player_is_seekable(m_vlcMediaPlayer);

    Error::printErrorMsg();

    return seekable;
}

float MediaPlayer::position() const
{
    if (!m_vlcMediaPlayer) {
        return -1;
    }

    return m_position;
}

float MediaPlayer::rate() const
{
    if (!m_vlcMediaPlayer) {
        return -1;
    }

    return m_rate;
}

QSize MediaPlayer::videoResolution() const
{
    if (!m_vlcMediaPlayer) {
        return QSize();
    }

    return m_videoResolution;
}

float MediaPlayer::sampleAspectRatio() const
{
    if (!m_vlcMediaPlayer) {
        return 0.0;
    }

    float sar = 0.0;

    return sar;
}

Enum::PlaybackState MediaPlayer::playbackState() const
{
    return m_playerState;
}

void MediaPlayer::setTime(int time)
{
    if (!(playbackState() == Enum::Playing || playbackState() == Enum::Paused)) {
        return;
    }

    bool changed = libvlc_media_player_set_time(m_vlcMediaPlayer, time, false);

    if (playbackState() == Enum::Paused && changed == 0) {
        emit timeChanged(time);
    }

    Error::printErrorMsg();
}

void MediaPlayer::setPosition(float pos)
{
    libvlc_media_player_set_position(m_vlcMediaPlayer, pos, false);

    Error::printErrorMsg();
}

void MediaPlayer::setPlaybackRate(float rate)
{
    libvlc_media_player_set_rate(m_vlcMediaPlayer, rate);

    Error::printErrorMsg();
}

void MediaPlayer::play()
{
    if (!m_vlcMediaPlayer) {
        return;
    }

    //#if defined(Q_OS_WIN32)
    //    libvlc_media_player_set_hwnd(_vlcMediaPlayer, (void *)_currentWId);
    //#elif defined(Q_OS_DARWIN)
    //    libvlc_media_player_set_nsobject(_vlcMediaPlayer, (void *)_currentWId);
    //#elif defined(Q_OS_UNIX)
    //    libvlc_media_player_set_xwindow(m_vlcMediaPlayer, 0);
    //#endif

    libvlc_media_player_play(m_vlcMediaPlayer);

    Error::printErrorMsg();
}

void MediaPlayer::pause()
{
    if (!m_vlcMediaPlayer) {
        return;
    }

    if (libvlc_media_player_can_pause(m_vlcMediaPlayer)) {
        libvlc_media_player_set_pause(m_vlcMediaPlayer, true);
    }

    Error::printErrorMsg();
}

void MediaPlayer::togglePause()
{
    if (!m_vlcMediaPlayer) {
        return;
    }

    if (libvlc_media_player_can_pause(m_vlcMediaPlayer)) {
        libvlc_media_player_pause(m_vlcMediaPlayer);
    }

    Error::printErrorMsg();
}

void MediaPlayer::resume()
{
    if (!m_vlcMediaPlayer) {
        return;
    }

    if (libvlc_media_player_can_pause(m_vlcMediaPlayer)) {
        libvlc_media_player_set_pause(m_vlcMediaPlayer, false);
    }

    Error::printErrorMsg();
}

void MediaPlayer::stop()
{
    if (!m_vlcMediaPlayer) {
        return;
    }

    libvlc_media_player_stop_async(m_vlcMediaPlayer);

    Error::printErrorMsg();
}

void MediaPlayer::parse()
{
    libvlc_event_manager_t *eventManager = libvlc_media_event_manager(currentMediaCore());
    libvlc_event_attach(eventManager, libvlc_MediaParsedChanged, libvlc_callback, this);
    if (libvlc_media_parse_request(m_vlcInstance, currentMediaCore(), libvlc_media_parse_local, 0) == -1){
        m_videoResolution = QSize();
        emit videoResolutionChanged(m_videoResolution);
    }
}

void MediaPlayer::checkParseStatus()
{
    libvlc_media_parsed_status_t status = libvlc_media_get_parsed_status(currentMediaCore());
    switch (status) {
    case libvlc_media_parsed_status_done: {
        libvlc_media_tracklist_t *tracklist = libvlc_media_get_tracklist(currentMediaCore(), libvlc_track_video);
        if (libvlc_media_tracklist_count(tracklist) > 0) {
            const libvlc_media_track_t *p_track = libvlc_media_tracklist_at(tracklist, 0);
            QSize resolution = QSize(p_track->video->i_width, p_track->video->i_height);
            if (resolution.isValid()) {
                m_videoResolution = resolution;
                emit videoResolutionChanged(m_videoResolution);
            }
        }
        else{
            m_videoResolution = QSize();
            emit videoResolutionChanged(m_videoResolution);
        }
        break;
    }
    case libvlc_media_parsed_status_skipped:
    case libvlc_media_parsed_status_failed:
    case libvlc_media_parsed_status_timeout:
        m_videoResolution = QSize();
        emit videoResolutionChanged(m_videoResolution);
        break;
    default:
        break;
    }
}

void MediaPlayer::libvlc_callback(const libvlc_event_t *event, void *data)
{
    auto *core = static_cast<MediaPlayer *>(data);

    switch (event->type) {
    case libvlc_MediaPlayerMediaChanged:
        core->parse();
        emit core->mediaChanged(event->u.media_player_media_changed.new_media);
        break;
    case libvlc_MediaPlayerNothingSpecial:
        QMetaObject::invokeMethod(core, [core]() {
            core->m_playerState = Enum::Idle;
            emit core->nothingSpecial();
        });
        break;
    case libvlc_MediaPlayerOpening:
        QMetaObject::invokeMethod(core, [core]() {
            core->m_playerState = Enum::Opening;
            emit core->opening();
        });
        break;
    case libvlc_MediaPlayerBuffering:
        emit core->buffering(event->u.media_player_buffering.new_cache);
        emit core->buffering(qRound(event->u.media_player_buffering.new_cache));
        break;
    case libvlc_MediaPlayerPlaying: {
        QMetaObject::invokeMethod(core, [core]() {
            core->m_playerState = Enum::Playing;
            emit core->playing();
        });
        break;
    }
    case libvlc_MediaPlayerPaused:
        QMetaObject::invokeMethod(core, [core]() {
            core->m_playerState = Enum::Paused;
            emit core->paused();
        });
        break;
    case libvlc_MediaPlayerStopping:
        QMetaObject::invokeMethod(core, [core]() {
            core->m_playerState = Enum::Stopping;
            emit core->paused();
        });
        break;
    case libvlc_MediaPlayerStopped:
        QMetaObject::invokeMethod(core, [core]() {
            core->m_playerState = Enum::Stopped;
            core->m_time = 0;
            emit core->timeChanged(core->m_time);
            core->m_length = 0;
            emit core->lengthChanged(core->m_length);
            core->m_position = 0.0f;
            emit core->positionChanged(core->m_position);
            core->m_rate = 1.0f;
            emit core->rateChanged(core->m_rate);
            emit core->stopped();
        });
        break;
    case libvlc_MediaPlayerForward:
        emit core->forward();
        break;
    case libvlc_MediaPlayerBackward:
        emit core->backward();
        break;
    case libvlc_MediaPlayerEncounteredError:
        QMetaObject::invokeMethod(core, [core]() {
            core->m_playerState = Enum::Error;
            emit core->error();
        });
        break;
    case libvlc_MediaPlayerTimeChanged:
        QMetaObject::invokeMethod(core, [core, new_time = event->u.media_player_time_changed.new_time]() {
            core->m_time = new_time;
            emit core->timeChanged(core->m_time);
        });
        break;
    case libvlc_MediaPlayerPositionChanged: {
        QMetaObject::invokeMethod(core, [core, new_position = event->u.media_player_position_changed.new_position]() {
            core->m_position = new_position;
            emit core->positionChanged(core->m_position);
        });
        break;
    }
    case libvlc_MediaPlayerSeekableChanged:
        emit core->seekableChanged(event->u.media_player_seekable_changed.new_seekable);
        break;
    case libvlc_MediaPlayerPausableChanged:
        emit core->pausableChanged(event->u.media_player_pausable_changed.new_pausable);
        break;
    case libvlc_MediaPlayerLengthChanged:
        QMetaObject::invokeMethod(core, [core, new_length = event->u.media_player_length_changed.new_length]() {
            core->m_length = new_length;
            emit core->lengthChanged(core->m_length);
        });
        break;
    case libvlc_MediaParsedChanged:
        core->checkParseStatus();
        break;
    default:
        break;
    }
}

void MediaPlayer::createCoreConnections()
{
    QList<libvlc_event_e> events;

    events << libvlc_MediaPlayerMediaChanged << libvlc_MediaPlayerNothingSpecial << libvlc_MediaPlayerOpening
           << libvlc_MediaPlayerBuffering << libvlc_MediaPlayerPlaying << libvlc_MediaPlayerPaused
           << libvlc_MediaPlayerStopped << libvlc_MediaPlayerForward << libvlc_MediaPlayerBackward
           << libvlc_MediaPlayerEncounteredError << libvlc_MediaPlayerTimeChanged << libvlc_MediaPlayerPositionChanged
           << libvlc_MediaPlayerSeekableChanged << libvlc_MediaPlayerPausableChanged << libvlc_MediaPlayerSnapshotTaken
           << libvlc_MediaPlayerLengthChanged << libvlc_MediaPlayerVout;

    for (auto &event : events) {
        libvlc_event_attach(m_vlcEvents, event, libvlc_callback, this);
    }
}

void MediaPlayer::removeCoreConnections()
{
    QList<libvlc_event_e> events;

    events << libvlc_MediaPlayerMediaChanged << libvlc_MediaPlayerNothingSpecial << libvlc_MediaPlayerOpening
           << libvlc_MediaPlayerBuffering << libvlc_MediaPlayerPlaying << libvlc_MediaPlayerPaused
           << libvlc_MediaPlayerStopped << libvlc_MediaPlayerForward << libvlc_MediaPlayerBackward
           << libvlc_MediaPlayerEncounteredError << libvlc_MediaPlayerTimeChanged << libvlc_MediaPlayerPositionChanged
           << libvlc_MediaPlayerSeekableChanged << libvlc_MediaPlayerPausableChanged << libvlc_MediaPlayerSnapshotTaken
           << libvlc_MediaPlayerLengthChanged << libvlc_MediaPlayerVout;

    for (auto &event : events) {
        libvlc_event_detach(m_vlcEvents, event, libvlc_callback, this);
    }
}

}  // namespace Vlc
