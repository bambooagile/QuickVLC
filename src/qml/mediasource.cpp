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

#include "mediasource.h"

#include "core/mediaplayer.h"
#include "videooutput.h"

MediaSource::MediaSource(QQuickItem *parent) : QQuickItem { parent }
{
    m_videoStream = new VideoStream(parent);

    connect(this, &QQuickItem::windowChanged, this, &MediaSource::handleWindowChanged);
}

MediaSource::~MediaSource()
{
    delete m_videoStream;
}

void MediaSource::setPlayer(Vlc::MediaPlayer *player)
{
    m_videoStream->init(player);
}

void MediaSource::removePlayer()
{
    m_videoStream->deinit();
}

void MediaSource::registerVideoOutput(VideoOutput *output)
{
    m_videoStream->registerVideoOutput(output);
}

void MediaSource::deregisterVideoOutput(VideoOutput *output)
{
    m_videoStream->deregisterVideoOutput(output);
}

void MediaSource::handleWindowChanged(QQuickWindow *window)
{
    m_videoStream->windowChanged(window);
    if (window) {
        connect(window, &QQuickWindow::beforeSynchronizing, this, &MediaSource::sync, Qt::DirectConnection);
        connect(window, &QQuickWindow::sceneGraphInvalidated, this, &MediaSource::cleanup, Qt::DirectConnection);
    }
}

void MediaSource::sync()
{
    m_videoStream->initContext();
}

void MediaSource::cleanup()
{
}

std::shared_ptr<Vlc::AbstractVideoFrame> MediaSource::getVideoFrame()
{
    return m_videoStream->getVideoFrame();
}