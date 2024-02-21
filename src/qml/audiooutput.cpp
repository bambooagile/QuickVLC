/******************************************************************************
 * This file is part of QuickVLC - Qt and libvlc connection library.
 * Copyright (C) 2022 Alexey Avramchik (OU Bamboo group) <aa@bam-boo.eu>
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

#include "audiooutput.h"

AudioOutput::AudioOutput() : m_audio { nullptr }
{
    setFlag(QQuickItem::ItemHasContents, false);
}

AudioOutput::~AudioOutput()
{
    delete m_audio;
}

void AudioOutput::init(Vlc::MediaPlayer *mediaPlayer)
{
    m_audio = new Vlc::Audio(mediaPlayer);
	connect(m_audio, &Vlc::Audio::muteChanged, this, &AudioOutput::mutedChanged);
	connect(m_audio, &Vlc::Audio::volumeChanged, this, &AudioOutput::volumeChanged);
}

bool AudioOutput::muted()
{
    if (m_audio != nullptr) {
        return m_audio->muted();
    }

    return false;
}

void AudioOutput::setMuted(bool isMuted)
{
    if (m_audio != nullptr) {
        m_audio->setMuted(isMuted);
    }
}

float AudioOutput::volume()
{
    if (m_audio != nullptr) {
        auto percentVolume = m_audio->volume();

        return percentVolume / 100.0f;
    }

    return 0.0f;
}

void AudioOutput::setVolume(float newVolume)
{
    if (m_audio != nullptr) {
        int percentVolume = newVolume * 100;

        m_audio->setVolume(percentVolume);
    }
}
