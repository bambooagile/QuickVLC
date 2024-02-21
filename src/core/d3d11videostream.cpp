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

#include "d3d11videostream.h"
#include "abstractvideoframe.h"

#include <QDebug>
#include <QtQuick/qsgtexture_platform.h>
#include <QtQuick/qsgrendererinterface.h>
#include <videoframepool.h>

#include <d3d11_1.h>
#include <dxgi1_2.h>
#include <wrl.h>

using namespace Microsoft::WRL;

namespace Vlc {

static const int POOL_SIZE = 7;
static const int INFLIGHT_RESERVED = 3;

class D3DVideoFrame : public AbstractVideoFrame
{
public:
    virtual ~D3DVideoFrame()
    {
        cleanup();
    }

    void cleanup()
    {
        m_renderTarget.Reset();
        m_qtTexture.Reset();
        if (m_sharedHandle) {
            CloseHandle(m_sharedHandle);
            m_sharedHandle = nullptr;
        }
        clear();
    }

    bool init(QQuickWindow *window, int width, int height, ComPtr<ID3D11Device> &qtD3DDevice,
        ComPtr<ID3D11Device> &vlcD3DDevice)
    {
        setSize({ width, height });

        m_window = window;

        HRESULT hr;

        D3D11_TEXTURE2D_DESC texDesc = {};
        texDesc.MipLevels = 1;
        texDesc.SampleDesc.Count = 1;

        texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
        texDesc.Usage = D3D11_USAGE_DEFAULT;
        texDesc.CPUAccessFlags = 0;
        texDesc.ArraySize = 1;
        texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        texDesc.Height = height;
        texDesc.Width = width;
        texDesc.MiscFlags = D3D11_RESOURCE_MISC_SHARED | D3D11_RESOURCE_MISC_SHARED_NTHANDLE;

        hr = qtD3DDevice->CreateTexture2D(&texDesc, nullptr, &m_qtTexture);
        if (FAILED(hr)) {
            return false;
            qWarning() << "fail to create D3D Qt texture";

        }

        // share texture between VLC and Qt
        {
            ComPtr<IDXGIResource1> sharedResource;
            m_qtTexture.As(&sharedResource);
            hr = sharedResource->CreateSharedHandle(
                nullptr, DXGI_SHARED_RESOURCE_READ | DXGI_SHARED_RESOURCE_WRITE, nullptr, &m_sharedHandle);
            if (FAILED(hr)) {
                qWarning() << "fail to create texture shared handle";
                return false;
            }
        }

        ComPtr<ID3D11Texture2D> texture2D;
        {
            ComPtr<ID3D11Device1> device1;
            vlcD3DDevice.As(&device1);
            hr = device1->OpenSharedResource1(m_sharedHandle, IID_PPV_ARGS(&texture2D));
            if (FAILED(hr)) {
                qWarning() << "fail to open shared handle";
                return false;
            }
                
        }

        D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc =
            CD3D11_RENDER_TARGET_VIEW_DESC(
                D3D11_RTV_DIMENSION_TEXTURE2D, 
                texDesc.Format);
        vlcD3DDevice->CreateRenderTargetView(texture2D.Get(), &renderTargetViewDesc, &m_renderTarget);
        if (FAILED(hr) || !m_renderTarget) {
            qWarning() << "fail to create render target view";
            return false;
        }

        return true;
    }

    bool isFlipped() const override
    {
        return false;
    }

    QSGTexture *getQSGTexture() override
    {
        if (!m_qtTexture)
            return nullptr;

        return QNativeInterface::QSGD3D11Texture::fromNative(m_qtTexture.Get(), m_window, size());
    }

public:
    QQuickWindow *m_window = nullptr;
    ComPtr<ID3D11Texture2D> m_qtTexture;
    HANDLE m_sharedHandle = nullptr;  // handle of the texture used by VLC and the app
    ComPtr<ID3D11RenderTargetView> m_renderTarget;


};

static const FLOAT BLACK_RGBA[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
static const FLOAT RED_RGBA[4] = { 1.0f, 0.0f, 0.0f, 1.0f };

struct D3D11VideoStream::D3D11VideoStreamPrivate 
{
    //Qt resources
    ComPtr<ID3D11Device> qtD3DDevice;
    QQuickWindow *window = nullptr;

    //VLC resources
    ComPtr<ID3D11Device> vlcD3DDevice;
    ComPtr<ID3D11DeviceContext> vlcD3DContext;


    std::shared_ptr<PooledVideoFrame> m_renderingFrame;
    std::shared_ptr<PooledVideoFrame> m_readyFrame;

    std::shared_ptr<VideoFramePool> m_pool;

    unsigned width = 800;
    unsigned height = 0;

    QMutex text_lock;
};

D3D11VideoStream::D3D11VideoStream(QQuickItem *parent) 
    : AbstractVideoStream(parent)
{
    m_priv = std::make_unique<D3D11VideoStreamPrivate>();
}

D3D11VideoStream::~D3D11VideoStream()
{
    cleanup();
}

void D3D11VideoStream::windowChanged(QQuickWindow *window)
{
    m_priv->window = window;

}

void D3D11VideoStream::initContext()
{
    Q_ASSERT(QSGRendererInterface::isApiRhiBased(QSGRendererInterface::Direct3D11));

    if (m_priv->vlcD3DDevice)
        return;

    HRESULT hr;

    QSGRendererInterface *qri = m_priv->window->rendererInterface();
    assert(qri->isApiRhiBased(QSGRendererInterface::Direct3D11));
    m_priv->qtD3DDevice =
        static_cast<ID3D11Device *>(qri->getResource(m_priv->window, QSGRendererInterface::DeviceResource));

    //Create vlc D3D device

    UINT creationFlags = D3D11_CREATE_DEVICE_VIDEO_SUPPORT;
    //| D3D11_CREATE_DEVICE_DEBUG;
    
    D3D_FEATURE_LEVEL requestedFeaturesLevels[] = {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
    };

    hr = D3D11CreateDevice(
        nullptr, // Adapter
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr, // Module
        creationFlags,
        requestedFeaturesLevels, 
        sizeof(requestedFeaturesLevels) / sizeof(*requestedFeaturesLevels),
        D3D11_SDK_VERSION,
        &m_priv->vlcD3DDevice,
        nullptr, //actual feature level
        &m_priv->vlcD3DContext
    );

    if (FAILED(hr)) {
        qWarning() << "can't create D3D device";
    }

    ComPtr<ID3D10Multithread> d3dMultithread;
    hr = m_priv->vlcD3DContext.As(&d3dMultithread);
    if (FAILED(hr)) {
        qWarning() << "can't setup D3D multithread protection";
    } else {
        d3dMultithread->SetMultithreadProtected(TRUE);
    }
    

    m_videoReady.release();
}

libvlc_video_engine_t D3D11VideoStream::videoEngine()
{
    return libvlc_video_engine_d3d11;
}

std::shared_ptr<AbstractVideoFrame> D3D11VideoStream::getVideoFrame()
{
    QMutexLocker locker(&m_priv->text_lock);
    return m_priv->m_readyFrame;
}

bool D3D11VideoStream::updateOutput(const libvlc_video_render_cfg_t *cfg, libvlc_video_output_cfg_t *render_cfg)
{
    if (m_priv->width != cfg->width || m_priv->height != cfg->height)
    {
        m_priv->width = cfg->width;
        m_priv->height = cfg->height;

        QMutexLocker locker(&m_priv->text_lock);
        m_priv->m_pool = std::make_shared<VideoFramePool>(INFLIGHT_RESERVED);
        for (int i = 0; i < POOL_SIZE; i++) {
            D3DVideoFrame *frame = new D3DVideoFrame();
            frame->init(
                m_priv->window, m_priv->width, m_priv->height, m_priv->qtD3DDevice, m_priv->vlcD3DDevice);
            m_priv->m_pool->enqueue(frame);
        }

        AbstractVideoFrame* renderFrame = m_priv->m_pool->pop(0);
        assert(renderFrame);
        m_priv->m_renderingFrame = std::make_shared<PooledVideoFrame>(renderFrame, m_priv->m_pool);
        m_priv->m_readyFrame.reset();
    }

    render_cfg->dxgi_format = DXGI_FORMAT_R8G8B8A8_UNORM;
    render_cfg->full_range = true;
    render_cfg->colorspace = libvlc_video_colorspace_BT709;
    render_cfg->primaries = libvlc_video_primaries_BT709;
    render_cfg->transfer = libvlc_video_transfer_func_SRGB;
    render_cfg->orientation = libvlc_video_orient_top_left;

    return true;
}

bool D3D11VideoStream::setup(const libvlc_video_setup_device_cfg_t *cfg, libvlc_video_setup_device_info_t *out)
{
    Q_UNUSED(cfg)

    /* Wait for rendering view to be ready. */
    m_videoReady.acquire();

    out->d3d11.device_context = m_priv->vlcD3DContext.Get();

    m_priv->width = 0;
    m_priv->height = 0;

    return true;
}

void D3D11VideoStream::cleanup()
{
    m_videoReady.release();
    {
        QMutexLocker locker(&m_priv->text_lock);

        m_priv->m_renderingFrame.reset();
        m_priv->m_readyFrame.reset();
        m_priv->m_pool.reset();
    }

    emit frameUpdated();
}

void D3D11VideoStream::swap()
{
    {
        QMutexLocker locker(&m_priv->text_lock);
        m_priv->m_readyFrame = m_priv->m_renderingFrame;
        AbstractVideoFrame* frame = m_priv->m_pool->pop(0);
        assert(frame);
        m_priv->m_renderingFrame = std::make_shared<PooledVideoFrame>(frame, m_priv->m_pool);
    }
    emit frameUpdated();
}

bool D3D11VideoStream::makeCurrent(bool isCurrent)
{
    if (isCurrent) {
        m_priv->vlcD3DContext->ClearRenderTargetView(
            m_priv->m_renderingFrame->as<D3DVideoFrame>()->m_renderTarget.Get(), BLACK_RGBA);
    }
    return true;
}

bool D3D11VideoStream::selectPlane(size_t plane, void *outputPtr)
{
    if (plane != 0)
        return false;

    ID3D11RenderTargetView **output = static_cast<ID3D11RenderTargetView **>(outputPtr);
    *output = m_priv->m_renderingFrame->as<D3DVideoFrame>()->m_renderTarget.Get();

    return true;
}

void *D3D11VideoStream::getProcAddress(const char *current)
{
    //N/A OpenGL only
    return nullptr;
}

}  // namespace Vlc
