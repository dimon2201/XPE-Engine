#ifdef DX11

#include <d3d11.h>

namespace xpe {

    namespace core {

        void cHardwareManager::UpdateGpuStats(void* device)
        {
            GPU.MaxTexture2dArray  = D3D11_REQ_TEXTURE2D_ARRAY_AXIS_DIMENSION;
            GPU.MaxAnisotropyLevel = D3D11_REQ_MAXANISOTROPY;
            GPU.MaxRenderTargetsPerStage = D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT;
            D3D11_FEATURE_DATA_THREADING featureThreading = {};
            ((ID3D11Device*) device)->CheckFeatureSupport(D3D11_FEATURE_THREADING, &featureThreading, sizeof(featureThreading));
            GPU.IsConcurrentCreatesSupported = featureThreading.DriverConcurrentCreates == TRUE;
            GPU.IsCommandListSupported = featureThreading.DriverCommandLists == TRUE;
            GPU.MaxThreadGroupsPerDimension = D3D11_CS_DISPATCH_MAX_THREAD_GROUPS_PER_DIMENSION;
            GPU.MaxThreadsPerGroup = D3D11_CS_THREAD_GROUP_MAX_THREADS_PER_GROUP;
            GPU.MaxThreadGroup = {
                    D3D11_CS_THREAD_GROUP_MAX_X,
                    D3D11_CS_THREAD_GROUP_MAX_Y,
                    D3D11_CS_THREAD_GROUP_MAX_Z
            };
            GPU.MaxUnorderedAccessViews = D3D11_PS_CS_UAV_REGISTER_COUNT;
        }

    }

}

#endif // DX11