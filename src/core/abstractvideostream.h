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

#include <memory>

#include <vlc/vlc.h>

#include <QObject>
#include "core_shared_export.h"

class QQuickWindow;

namespace Vlc {

class MediaPlayer;
class AbstractVideoFrame;

class QUICKVLC_CORE_EXPORT AbstractVideoStream : public QObject
{
    Q_OBJECT
public:
    explicit AbstractVideoStream(QObject *parent = nullptr);
    virtual ~AbstractVideoStream();

    void setCallbacks(Vlc::MediaPlayer *player);

    void unsetCallbacks(Vlc::MediaPlayer *player);

    virtual void windowChanged(QQuickWindow *window) = 0;

    virtual std::shared_ptr<AbstractVideoFrame> getVideoFrame() = 0;

signals:
    void frameUpdated();

public slots:
    virtual void initContext() = 0;

protected:
    virtual libvlc_video_engine_t videoEngine() = 0;
    virtual bool updateOutput(const libvlc_video_render_cfg_t *cfg, libvlc_video_output_cfg_t *render_cfg) = 0;
    virtual bool setup(const libvlc_video_setup_device_cfg_t *cfg, libvlc_video_setup_device_info_t *out) = 0;
    virtual void cleanup() = 0;
    virtual void swap() = 0;
    virtual bool makeCurrent(bool isCurrent) = 0;
    virtual bool selectPlane(size_t plane, void *output) = 0;
    virtual void *getProcAddress(const char *current) = 0;

private:
    static bool setup_cb(
        void **opaque, const libvlc_video_setup_device_cfg_t *cfg, libvlc_video_setup_device_info_t *out);
    static void cleanup_cb(void *opaque);
    static bool update_output_cb(
        void *opaque, const libvlc_video_render_cfg_t *cfg, libvlc_video_output_cfg_t *render_cfg);
    static void swap_cb(void *opaque);
    static bool make_current_cb(void *opaque, bool current);
    static bool select_plane_cb(void *opaque, size_t plane, void *output);
    static void *get_proc_address_cb(void *opaque, const char *current);
};

}  // namespace Vlc
