#pragma once

#include <rendering/lighting/direct_light.h>
#include <rendering/lighting/point_light.h>
#include <rendering/lighting/spot_light.h>

namespace xpe {

    namespace render {

        class ENGINE_API LightManager final {

        public:
            static const usize K_DIRECT_LIGHT_COUNT = 10;
            static const usize K_POINT_LIGHT_COUNT = 1000;
            static const usize K_SPOT_LIGHT_COUNT = 1000;

        public:
            static void Init(Context* context);
            static void Free();

            static void InitLight(DirectLight& light);
            static void InitLight(PointLight& light);
            static void InitLight(SpotLight& light);

            static void FreeLight(DirectLight& light);
            static void FreeLight(PointLight& light);
            static void FreeLight(SpotLight& light);

            static vector<DirectLightBufferData>& DirectList();
            static vector<PointLightBufferData>& PointList();
            static vector<SpotLightBufferData>& SpotList();

            static DirectLightBuffer* GetDirectBuffer();
            static PointLightBuffer* GetPointBuffer();
            static SpotLightBuffer* GetSpotBuffer();

            static void UpdateLight(const DirectLight& light);
            static void UpdateLight(const PointLight& light);
            static void UpdateLight(const SpotLight& light);

        private:
            static Context* s_Context;
            static DirectLightBuffer s_DirectLightBuffer;
            static PointLightBuffer s_PointLightBuffer;
            static SpotLightBuffer s_SpotLightBuffer;

        };

    }

}