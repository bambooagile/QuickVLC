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

#include "openglvideoframe.h"
#include <QtQuick/qsgtexture_platform.h>

namespace Vlc {

OpenGLVideoFrame::OpenGLVideoFrame(int width, int height, QQuickWindow *window) 
    : AbstractVideoFrame()
    , m_fbo(width, height)
    , m_window(window)
{
    setSize(m_fbo.size());
}

OpenGLVideoFrame::~OpenGLVideoFrame()
{
}

GLuint OpenGLVideoFrame::texture() const
{
    return m_fbo.texture();
}

bool OpenGLVideoFrame::isFlipped() const
{
    //OpenGL textures are upside down
    return true;
}

QSGTexture *OpenGLVideoFrame::getQSGTexture()
{
    return QNativeInterface::QSGOpenGLTexture::fromNative(
        m_fbo.texture(), m_window, size());
}

}  // namespace Vlc
