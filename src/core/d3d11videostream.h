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

#pragma once

#include <QMutex>
#include <QSemaphore>
#include <QtQuick/QQuickItem>

#include "abstractvideostream.h"
#include "core_shared_export.h"

namespace Vlc {

class AbstractVideoFrame;

class QUICKVLC_CORE_EXPORT D3D11VideoStream : public AbstractVideoStream
{

public:
    explicit D3D11VideoStream(QQuickItem *parent = nullptr);
    ~D3D11VideoStream();

    void windowChanged(QQuickWindow *window) override;
    std::shared_ptr<AbstractVideoFrame> getVideoFrame() override;
    
    void initContext() override;

private:
    libvlc_video_engine_t videoEngine() override;
    bool updateOutput(const libvlc_video_render_cfg_t *cfg, libvlc_video_output_cfg_t *render_cfg) override;
    bool setup(const libvlc_video_setup_device_cfg_t *cfg, libvlc_video_setup_device_info_t *out) override;
    void cleanup() override;
    void swap() override;
    bool makeCurrent(bool isCurrent) override;
    bool selectPlane(size_t plane, void *output) override;
    void *getProcAddress(const char *current) override;

    QSemaphore m_videoReady;

    struct D3D11VideoStreamPrivate;
    std::unique_ptr<D3D11VideoStreamPrivate> m_priv;
};

}  // namespace Vlc
