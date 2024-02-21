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

#include <vlc/vlc.h>

#include <QObject>

#include "core_shared_export.h"
#include "vlc.h"

struct libvlc_event_t;
struct libvlc_event_manager_t;
struct libvlc_media_t;
struct libvlc_media_player_t;
struct libvlc_instance_t;

namespace Vlc {

class Instance;
class Media;

class QUICKVLC_CORE_EXPORT MediaPlayer : public QObject
{
    Q_OBJECT
public:
    explicit MediaPlayer(Instance *instance);

    ~MediaPlayer();

    libvlc_media_player_t *core() const;
    libvlc_event_manager_t *eventManager() const;

    int length() const;

    libvlc_media_t *currentMediaCore();

    void open(Media *media);

    void openOnly(Media *media);

    int time() const;

    bool seekable() const;

    float position() const;

    float rate() const;

    float sampleAspectRatio() const;

    float playbackRate() const;

    Enum::PlaybackState playbackState() const;

    QSize videoResolution() const;

public slots:
    void setTime(int time);

    void setPosition(float pos);

    void setPlaybackRate(float rate);

    void play();

    void pause();

    void togglePause();

    void resume();

    void stop();

signals:
    void backward();
    void buffering(float buffer);
    void buffering(int buffer);
    void forward();
    void lengthChanged(int length);
    void mediaChanged(libvlc_media_t *media);
    void pausableChanged(bool pausable);
    void positionChanged(float position);
    void seekableChanged(bool seekable);
    void timeChanged(int time);
    void rateChanged(float rate);
    void videoResolutionChanged(QSize resolution);

    // player states
    void nothingSpecial();
    void opening();
    void paused();
    void playing();
    void error();
    void stopping();
    void stopped();
    void playbackStateChanged();

private:
    static void libvlc_callback(const libvlc_event_t *event, void *data);

    void createCoreConnections();
    void removeCoreConnections();
    void parse();
    void checkParseStatus();

    Enum::PlaybackState m_playerState;

    libvlc_media_player_t *m_vlcMediaPlayer;
    libvlc_event_manager_t *m_vlcEvents;
    libvlc_instance_t *m_vlcInstance;

    qint64 m_time = 0;
    qint64 m_length = 0;
    double m_position = 0.0f;
    double m_rate = 1.0f;
    QSize m_videoResolution;

    Media *m_media;
};

}  // namespace Vlc
