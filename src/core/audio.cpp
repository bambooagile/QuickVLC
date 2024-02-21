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

#include "audio.h"

#include <vlc/vlc.h>

#include "error.h"
#include "mediaplayer.h"

namespace Vlc {

Audio::Audio(MediaPlayer *player)
    : QObject(player), m_mediaPlayer { player->core() }, m_eventManager { player->eventManager() }
{
    createCoreConnections();
}

Audio::~Audio()
{
    removeCoreConnections();
}

bool Vlc::Audio::muted() const
{
    bool isMuted = false;

    if (m_mediaPlayer != nullptr) {
        isMuted = libvlc_audio_get_mute(m_mediaPlayer);

        Error::printErrorMsg();
    }

    return isMuted;
}

void Audio::setMuted(bool isMuted)
{
    if (m_mediaPlayer != nullptr && isMuted != muted()) {
        libvlc_audio_set_mute(m_mediaPlayer, isMuted);

        Error::printErrorMsg();
    }
}

int Audio::volume() const
{
    int volume = -1;

    if (m_mediaPlayer != nullptr) {
        volume = libvlc_audio_get_volume(m_mediaPlayer);

        Error::printErrorMsg();
    }

    return volume;
}

void Audio::setVolume(int volume)
{
    if (m_mediaPlayer != nullptr /*&& volume != Audio::volume()*/) {
        libvlc_audio_set_volume(m_mediaPlayer, volume);

        Error::printErrorMsg();
    }
}

void Audio::libvlc_callback(const libvlc_event_t *event, void *data)
{
    auto *core = static_cast<Audio *>(data);

    switch (event->type) {
    case libvlc_MediaPlayerMuted:
        emit core->muteChanged(true);
        break;
    case libvlc_MediaPlayerUnmuted:
        emit core->muteChanged(false);
    case libvlc_MediaPlayerAudioVolume:
        emit core->volumeChanged(event->u.media_player_audio_volume.volume);
        break;
    }
}

void Audio::createCoreConnections()
{
    QList<libvlc_event_e> events;

    events << libvlc_MediaPlayerMuted << libvlc_MediaPlayerUnmuted << libvlc_MediaPlayerAudioVolume;

    for (auto &event : events) {
        libvlc_event_attach(m_eventManager, event, libvlc_callback, this);
    }
}

void Audio::removeCoreConnections()
{
    QList<libvlc_event_e> events;

    events << libvlc_MediaPlayerMuted << libvlc_MediaPlayerUnmuted << libvlc_MediaPlayerAudioVolume;

    for (auto &event : events) {
        libvlc_event_detach(m_eventManager, event, libvlc_callback, this);
    }
}

}  // namespace Vlc
