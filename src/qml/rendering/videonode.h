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

#include <core/videoframe.h>

#include <QSGGeometryNode>

#include "rendering/videomaterial.h"

class VideoNode : public QSGGeometryNode
{
public:
    VideoNode();

    void setRect(const QRectF &rect, const QRectF &sourceRect);
    void updateFrame(const std::shared_ptr<Vlc::AbstractVideoFrame> &frame);

private:
    QSGGeometry m_geometry;
    VideoMaterial m_material;
};
