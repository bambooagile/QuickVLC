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

#include "videooutput.h"

#include "rendering/videonode.h"

VideoOutput::VideoOutput() : m_source { nullptr }, m_fillMode { Vlc::Enum::PreserveAspectFit }, m_frameUpdated { false }
{
    setFlag(QQuickItem::ItemHasContents, true);
}

VideoOutput::~VideoOutput()
{
    setSource(nullptr);
}

MediaSource *VideoOutput::source() const
{
    return m_source;
}

void VideoOutput::setSource(MediaSource *source)
{
    if (source == m_source) {
        return;
    }

    if (source) {
        source->deregisterVideoOutput(this);
    }

    m_source = source;

    if (m_source) {
        source->registerVideoOutput(this);
    }

    emit sourceChanged();
}

int VideoOutput::fillMode() const
{
    return m_fillMode;
}

void VideoOutput::setFillMode(int mode)
{
    if (m_fillMode == mode) {
        return;
    }

    m_fillMode = static_cast<Vlc::Enum::FillMode>(mode);

    update();

    emit fillModeChanged();
}

int VideoOutput::aspectRatio() const
{
    return m_aspectRatio;
}

void VideoOutput::setAspectRatio(int aspectRatio)
{
    if (m_aspectRatio == aspectRatio) {
        return;
    }

    m_aspectRatio = static_cast<Vlc::Enum::Ratio>(aspectRatio);

    update();

    emit aspectRatioChanged();
}

int VideoOutput::cropRatio() const
{
    return m_cropRatio;
}

void VideoOutput::setCropRatio(int cropRatio)
{
    if (m_cropRatio == cropRatio) {
        return;
    }

    m_cropRatio = static_cast<Vlc::Enum::Ratio>(cropRatio);

    update();

    emit cropRatioChanged();
}

void VideoOutput::presentFrame(const std::shared_ptr<Vlc::VideoFrame> &frame)
{
    m_frame = frame;

    m_frameUpdated = true;

    update();
}

QSGNode *VideoOutput::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *data)
{
    Q_UNUSED(data);

    VideoNode *node = static_cast<VideoNode *>(oldNode);

    if (!m_frame) {
        delete node;

        return nullptr;
    }

    if (!node) {
        node = new VideoNode();
    }

    if (m_frameUpdated) {
        m_frameUpdated = false;

        node->updateFrame(m_frame);
    }

    auto rects = calculateFillMode(m_frame->width(), m_frame->height());

    node->setRect(rects.out, rects.source);

    return node;
}

FrameFillRect VideoOutput::calculateFillMode(quint16 fw, quint16 fh)
{
    QRectF srcRect(0, 1.0, 1.0, -1.0);
    QRectF outRect(0, 0, width(), height());

    if (fillMode() != Vlc::Enum::Stretch) {
        qreal frameAspectTmp = qreal(fw) / fh;
        QSizeF aspectRatioSize = Vlc::Enum::ratioSize(m_aspectRatio);

        if (aspectRatioSize.width() != 0 && aspectRatioSize.height() != 0) {
            frameAspectTmp = aspectRatioSize.width() / aspectRatioSize.height();
        }

        QSizeF cropRatioSize = Vlc::Enum::ratioSize(m_cropRatio);

        if (cropRatioSize.width() != 0 && cropRatioSize.height() != 0) {
            const qreal cropAspect = cropRatioSize.width() / cropRatioSize.height();

            if (frameAspectTmp > cropAspect) {
                srcRect.setX((1.0 - cropAspect / frameAspectTmp) / 2);
                srcRect.setWidth(1.0 - srcRect.x() - srcRect.x());
            } else if (frameAspectTmp < cropAspect) {
                srcRect.setY((1.0 - frameAspectTmp / cropAspect) / 2);
                srcRect.setHeight(1.0 - srcRect.y() - srcRect.y());
            }

            frameAspectTmp = cropAspect;
        }

        const qreal itemAspect = width() / height();
        const qreal frameAspect = frameAspectTmp;

        if (fillMode() == Vlc::Enum::PreserveAspectFit) {
            qreal outWidth = width();
            qreal outHeight = height();

            if (frameAspect > itemAspect) {
                outHeight = outWidth / frameAspect;
            } else if (frameAspect < itemAspect) {
                outWidth = outHeight * frameAspect;
            }

            outRect = QRectF((width() - outWidth) / 2, (height() - outHeight) / 2, outWidth, outHeight);
        } else if (fillMode() == Vlc::Enum::PreserveAspectCrop) {
            if (frameAspect > itemAspect) {
                srcRect.setX((1.0 - itemAspect / frameAspect) / 2);
                srcRect.setWidth(1.0 - srcRect.x() - srcRect.x());
            } else if (frameAspect < itemAspect) {
                srcRect.setY((1.0 - frameAspect / itemAspect) / 2);
                srcRect.setHeight(1.0 - srcRect.y() - srcRect.y());
            }
        }
    }

    return FrameFillRect { srcRect, outRect };
}
