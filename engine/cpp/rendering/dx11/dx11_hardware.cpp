#ifdef DX11

#include <d3d11.h>

namespace xpe {

    namespace core {

        void Hardware::UpdateVideoStats()
        {
            s_VideoStats.MaxTexture2dArray  = D3D11_REQ_TEXTURE2D_ARRAY_AXIS_DIMENSION;
            s_VideoStats.MaxAnisotropyLevel = D3D11_REQ_MAXANISOTROPY;
            s_VideoStats.MaxRenderTargetsPerStage = D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT;
        }

    }

}

#endif // DX11