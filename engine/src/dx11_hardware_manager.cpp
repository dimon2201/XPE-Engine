#ifdef DX11

#include <d3d11.h>
#include <hardware_manager.hpp>

namespace xpe
{
    namespace core
    {
        sGpuInfo::sGpuInfo()
        {
            MaxTexture2dArray  = D3D11_REQ_TEXTURE2D_ARRAY_AXIS_DIMENSION;
            MaxAnisotropyLevel = D3D11_REQ_MAXANISOTROPY;
            MaxMipLevels = D3D11_REQ_MIP_LEVELS;
            MaxRenderTargetsPerStage = D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT;
            MaxThreadGroupsPerDimension = D3D11_CS_DISPATCH_MAX_THREAD_GROUPS_PER_DIMENSION;
            MaxThreadsPerGroup = D3D11_CS_THREAD_GROUP_MAX_THREADS_PER_GROUP;
            MaxThreadGroup = {
                    D3D11_CS_THREAD_GROUP_MAX_X,
                    D3D11_CS_THREAD_GROUP_MAX_Y,
                    D3D11_CS_THREAD_GROUP_MAX_Z
            };
            MaxUnorderedAccessViews = D3D11_PS_CS_UAV_REGISTER_COUNT;
        }

        void MHardware::UpdateGpuInfo(void* device)
        {
            D3D11_FEATURE_DATA_THREADING featureThreading = {};
            ((ID3D11Device*) device)->CheckFeatureSupport(D3D11_FEATURE_THREADING, &featureThreading, sizeof(featureThreading));
            GpuInfo.IsConcurrentCreatesSupported = featureThreading.DriverConcurrentCreates == TRUE;
            GpuInfo.IsCommandListSupported = featureThreading.DriverCommandLists == TRUE;
        }
    }
}

#endif // DX11