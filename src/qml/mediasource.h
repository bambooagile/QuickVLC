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

#include <QObject>
#include <QQmlParserStatus>
#include <QQuickItem>

#include "video/videostream.h"

namespace Vlc {
class MediaPlayer;
class AbstractVideoFrame;
};

class VideoOutput;

class MediaSource : public QQuickItem
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)

public:
    explicit MediaSource(QQuickItem *parent = nullptr);

    ~MediaSource();

    void setPlayer(Vlc::MediaPlayer *player);

    void removePlayer();

    virtual void registerVideoOutput(VideoOutput *output);

    virtual void deregisterVideoOutput(VideoOutput *output);

    std::shared_ptr<Vlc::AbstractVideoFrame> getVideoFrame();

public slots:
    void sync();
    void cleanup();

private slots:
    void handleWindowChanged(QQuickWindow *window);

private:
    //    void classBegin() override {};
    //    void componentComplete() override {};

    VideoStream *m_videoStream;
    bool m_contextReady;
};
