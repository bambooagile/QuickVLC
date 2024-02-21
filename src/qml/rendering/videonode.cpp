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

#include "videonode.h"

VideoNode::VideoNode()
{
    setFiltering(QSGTexture::Linear);
    setOwnsTexture(true);
}

void VideoNode::updateFrame(const std::shared_ptr<Vlc::AbstractVideoFrame> &frame)
{
    m_frame = frame;
    setTexture(frame->getQSGTexture());

    if (frame->isFlipped())
        setTextureCoordinatesTransform(MirrorVertically);
    else
        setTextureCoordinatesTransform(NoTransform);

    markDirty(QSGNode::DirtyMaterial);
}

QSize VideoNode::frameSize() const
{
    if (!m_frame)
        return {};
    return m_frame->size();
}
