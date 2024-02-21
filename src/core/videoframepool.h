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

#include <QQueue>
#include <QMutex>
#include <QWaitCondition>

#include "abstractvideoframe.h"
#include "core_shared_export.h"

namespace Vlc {

class QUICKVLC_CORE_EXPORT VideoFramePool
{
public:
    VideoFramePool(int reserved);
    ~VideoFramePool();

    AbstractVideoFrame* pop(unsigned long timeoutMS);
    void enqueue(AbstractVideoFrame *videoFrame);

    QMutex m_lock;
    QWaitCondition m_cond; 
    QQueue<AbstractVideoFrame*> m_queue;
    int m_reserved = 0;
};

class QUICKVLC_CORE_EXPORT PooledVideoFrame : public AbstractVideoFrame
{
public:
    explicit PooledVideoFrame(AbstractVideoFrame *videoFrame, std::shared_ptr<VideoFramePool> videoFramePool);

    ~PooledVideoFrame();

    bool isFlipped() const override;

    QSGTexture *getQSGTexture() override;

    template<typename T>
    T* as() const
    {
        return static_cast<T*>(m_frame);
    }

    virtual quint16 width() const override;
    virtual quint16 height() const override;

    virtual void setWidth(quint16 width) override;
    virtual void setHeight(quint16 height) override;

    virtual QSize size() const override;
    virtual void setSize(const QSize &size) override;

    virtual bool isValid() override;

    virtual void clear() override;

private:
    AbstractVideoFrame* m_frame = nullptr;
    std::shared_ptr<VideoFramePool> m_pool;
};



}  // namespace Vlc
