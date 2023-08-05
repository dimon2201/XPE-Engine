#pragma once

#include <rendering/lighting/direct_light.h>
#include <rendering/lighting/point_light.h>
#include <rendering/lighting/spot_light.h>

namespace xpe {

    namespace render {

        struct ENGINE_API LightStorage final {
            DirectLightBuffer DirectBuffer;
            PointLightBuffer PointBuffer;
            SpotLightBuffer SpotBuffer;

            ~LightStorage();
        };

        class ENGINE_API LightManager final {

        public:
            static const usize K_DIRECT_LIGHT_SIZE = 1;
            static const usize K_POINT_LIGHT_SIZE = 1000;
            static const usize K_SPOT_LIGHT_SIZE = 1000;

        public:
            static void Init();
            static void Free();

            static DirectLightBuffer* GetDirectBuffer();
            static PointLightBuffer* GetPointBuffer();
            static SpotLightBuffer* GetSpotBuffer();

            static void AddDirectLight(DirectLightComponent& light);
            static void AddPointLight(PointLightComponent& light);
            static void AddSpotLight(SpotLightComponent& light);

            static void RemoveDirectLight(const DirectLightComponent& light);
            static void RemovePointLight(const PointLightComponent& light);
            static void RemoveSpotLight(const SpotLightComponent& light);

            static void FlushDirectLight(const DirectLightComponent &light);
            static void FlushPointLight(const PointLightComponent &light);
            static void FlushSpotLight(const SpotLightComponent &light);

            static void FlushDirectLights();
            static void FlushPointLights();
            static void FlushSpotLights();

        private:
            static LightStorage* s_Storage;

        };

    }

}