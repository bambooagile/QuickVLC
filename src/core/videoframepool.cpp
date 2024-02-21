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

#include "videoframepool.h"

namespace Vlc {

PooledVideoFrame::PooledVideoFrame(AbstractVideoFrame *videoFrame, std::shared_ptr<VideoFramePool>  pool)
    : AbstractVideoFrame()
    , m_frame(videoFrame)
    , m_pool(pool)
{
}

PooledVideoFrame::~PooledVideoFrame()
{
    m_pool->enqueue(m_frame);
}

bool PooledVideoFrame::isFlipped() const
{
    return m_frame->isFlipped();
}

QSGTexture *PooledVideoFrame::getQSGTexture()
{
    return m_frame->getQSGTexture();
}

quint16 PooledVideoFrame::width() const
{
    return m_frame->width();
}


quint16 PooledVideoFrame::height() const
{
    return m_frame->height();
}

void PooledVideoFrame::setWidth(quint16 width)
{
    m_frame->setWidth(width);
}

void PooledVideoFrame::setHeight(quint16 height)
{
    m_frame->setHeight(height);
}

QSize PooledVideoFrame::size() const
{
    return m_frame->size();
}

void PooledVideoFrame::setSize(const QSize &size)
{
    m_frame->setSize(size);
}

bool PooledVideoFrame::isValid()
{
    return m_frame->isValid();
}

void PooledVideoFrame::clear()
{
    m_frame->clear();
}

VideoFramePool::VideoFramePool(int reserved)
    : m_reserved(reserved)
{
}

VideoFramePool::~VideoFramePool()
{
    for (AbstractVideoFrame *videoFrame : m_queue) 
    {
        delete videoFrame;
    }
    m_queue.clear();
}

AbstractVideoFrame* VideoFramePool::pop(unsigned long timeoutMS)
{
    QMutexLocker lock(&m_lock);

    while (m_queue.length() <= m_reserved)
    {
        bool signaled = m_cond.wait(&m_lock, timeoutMS);
        if (!signaled)
            return nullptr;
    }

    AbstractVideoFrame *videoFrame = m_queue.front();
    m_queue.pop_front();
    return videoFrame;
}

void VideoFramePool::enqueue(AbstractVideoFrame *videoFrame)
{
    QMutexLocker lock(&m_lock);
    m_queue.push_back(videoFrame);
    if (m_queue.length() >= m_reserved)
        m_cond.notify_one();
}

}  // namespace Vlc
