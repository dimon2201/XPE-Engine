#ifdef DX11

#include <d3d11.h>

namespace xpe {

    namespace core {

        void Hardware::UpdateGpuStats(void* device)
        {
            GPU.MaxTexture2dArray  = D3D11_REQ_TEXTURE2D_ARRAY_AXIS_DIMENSION;
            GPU.MaxAnisotropyLevel = D3D11_REQ_MAXANISOTROPY;
            GPU.MaxRenderTargetsPerStage = D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT;

            D3D11_FEATURE_DATA_THREADING featureThreading = {};
            ((ID3D11Device*) device)->CheckFeatureSupport(D3D11_FEATURE_THREADING, &featureThreading, sizeof(featureThreading));
            GPU.IsConcurrentCreatesSupported = featureThreading.DriverConcurrentCreates == TRUE;
            GPU.IsCommandListSupported = featureThreading.DriverCommandLists == TRUE;
        }

    }

}

#endif // DX11