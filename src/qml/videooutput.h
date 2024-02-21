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

#include <QOpenGLContext>
#include <QPointer>
#include <QQuickItem>
#include <QQuickWindow>
#include <QMutex>

#include "core/abstractvideoframe.h"
#include "core/vlc.h"
#include "mediasource.h"
#include "qml_shared_export.h"

struct FrameFillRect {
    QRectF source;
    QRectF out;
};

class QUICKVLC_QML_EXPORT VideoOutput : public QQuickItem
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(MediaSource *source READ source WRITE setSource NOTIFY sourceChanged)

    Q_PROPERTY(int fillMode READ fillMode WRITE setFillMode NOTIFY fillModeChanged)

    Q_PROPERTY(int aspectRatio READ aspectRatio WRITE setAspectRatio NOTIFY aspectRatioChanged)

    Q_PROPERTY(int cropRatio READ cropRatio WRITE setCropRatio NOTIFY cropRatioChanged)

public:
    VideoOutput();
    ~VideoOutput();

    MediaSource *source() const;

    void setSource(MediaSource *source);

    int fillMode() const;

    void setFillMode(int mode);

    int aspectRatio() const;

    void setAspectRatio(int aspectRatio);

    int cropRatio() const;

    void setCropRatio(int cropRatio);

    void presentFrame();

signals:
    void contextReady(QOpenGLContext *ctx);

    void initialized();

    void sourceChanged();

    void fillModeChanged();

    void aspectRatioChanged();

    void cropRatioChanged();

protected:
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *data) override;

private:
    FrameFillRect calculateFillMode(quint16 fw, quint16 fh);

    QPointer<MediaSource> m_source = nullptr;
     
    Vlc::Enum::FillMode m_fillMode = Vlc::Enum::PreserveAspectFit;
    Vlc::Enum::Ratio m_aspectRatio = Vlc::Enum::Ratio::Original;
    Vlc::Enum::Ratio m_cropRatio = Vlc::Enum::Ratio::Original;

    bool m_frameUpdated;
    QMutex m_frameMutex;
};
