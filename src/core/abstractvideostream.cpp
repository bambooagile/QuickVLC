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

#include "abstractvideostream.h"

#include "mediaplayer.h"

namespace Vlc {

static inline AbstractVideoStream *p_this(void *opaque)
{
    return static_cast<AbstractVideoStream *>(opaque);
}

static inline AbstractVideoStream *p_this(void **opaque)
{
    return static_cast<AbstractVideoStream *>(*opaque);
}

#define P_THIS p_this(opaque)

AbstractVideoStream::AbstractVideoStream(QObject *parent) : QObject(parent)
{
}

AbstractVideoStream::~AbstractVideoStream()
{
}

void AbstractVideoStream::setCallbacks(Vlc::MediaPlayer *player)
{
    libvlc_video_set_output_callbacks(player->core(), videoEngine(), setup_cb, cleanup_cb, nullptr, update_output_cb, swap_cb,
        make_current_cb, get_proc_address_cb, nullptr, select_plane_cb, this);
}

void AbstractVideoStream::unsetCallbacks(Vlc::MediaPlayer *player)
{
    if (player) {
        libvlc_video_set_output_callbacks(player->core(), libvlc_video_engine_disable, nullptr, nullptr, nullptr,
            nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
    }
}

bool AbstractVideoStream::update_output_cb(
    void *opaque, const libvlc_video_render_cfg_t *cfg, libvlc_video_output_cfg_t *render_cfg)
{
    return P_THIS->updateOutput(cfg, render_cfg);
}

bool AbstractVideoStream::setup_cb(
    void **opaque, const libvlc_video_setup_device_cfg_t *cfg, libvlc_video_setup_device_info_t *out)
{
    return P_THIS->setup(cfg, out);
}

void AbstractVideoStream::cleanup_cb(void *opaque)
{
    P_THIS->cleanup();
}

void AbstractVideoStream::swap_cb(void *opaque)
{
    P_THIS->swap();
}

bool AbstractVideoStream::make_current_cb(void *opaque, bool current)
{
    return P_THIS->makeCurrent(current);
}

bool AbstractVideoStream::select_plane_cb(void *opaque, size_t plane, void *output)
{
    return P_THIS->selectPlane(plane, output);
}

void *AbstractVideoStream::get_proc_address_cb(void *opaque, const char *current)
{
    return P_THIS->getProcAddress(current);
}

}  // namespace Vlc
