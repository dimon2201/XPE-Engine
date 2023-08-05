#include <rendering/lighting/light_manager.h>

namespace xpe {

    namespace render {

        LightStorage* LightManager::s_Storage = nullptr;

        LightStorage::~LightStorage() {
            DirectBuffer.Free();
            PointBuffer.Free();
            SpotBuffer.Free();
        }

        void LightManager::Init() {
            LogInfo("LightManager::Init()");

            s_Storage = new LightStorage();

            s_Storage->DirectBuffer = DirectLightBuffer(0);
            s_Storage->DirectBuffer.Reserve(K_DIRECT_LIGHT_SIZE);

            s_Storage->PointBuffer = PointLightBuffer(0);
            s_Storage->PointBuffer.Reserve(K_POINT_LIGHT_SIZE);

            s_Storage->SpotBuffer = SpotLightBuffer(0);
            s_Storage->SpotBuffer.Reserve(K_SPOT_LIGHT_SIZE);

            LogInfo("LightManager initialized");
        }

        void LightManager::Free() {
            LogInfo("LightManager::Free()");
            delete s_Storage;
        }

        DirectLightBuffer* LightManager::GetDirectBuffer() {
            return &s_Storage->DirectBuffer;
        }

        PointLightBuffer* LightManager::GetPointBuffer() {
            return &s_Storage->PointBuffer;
        }

        SpotLightBuffer* LightManager::GetSpotBuffer() {
            return &s_Storage->SpotBuffer;
        }

        void LightManager::AddDirectLight(DirectLightComponent &light) {
            DirectLightBufferData data;
            data.Position = light.Position;
            data.Color = light.Color;
            light.Index = s_Storage->DirectBuffer.Add(data);
        }

        void LightManager::AddPointLight(PointLightComponent &light) {
            PointLightBufferData data;
            data.Position = light.Position;
            data.Color = light.Color;
            data.Constant = light.Constant;
            data.Linear = light.Linear;
            data.Quadratic = light.Quadratic;
            light.Index = s_Storage->PointBuffer.Add(data);
        }

        void LightManager::AddSpotLight(SpotLightComponent &light) {
            SpotLightBufferData data;
            data.Position = light.Position;
            data.Direction = light.Direction;
            data.Color = light.Color;
            data.Cutoff = light.Cutoff;
            data.Outer = light.Outer;
            light.Index = s_Storage->SpotBuffer.Add(data);
        }

        void LightManager::RemoveDirectLight(const DirectLightComponent &light) {
            s_Storage->DirectBuffer.RemoveAt(light.Index);
        }

        void LightManager::RemovePointLight(const PointLightComponent &light) {
            s_Storage->PointBuffer.RemoveAt(light.Index);
        }

        void LightManager::RemoveSpotLight(const SpotLightComponent &light) {
            s_Storage->SpotBuffer.RemoveAt(light.Index);
        }

        void LightManager::FlushDirectLight(const DirectLightComponent &light) {
            DirectLightBufferData data;
            data.Position = light.Position;
            data.Color = light.Color;
            s_Storage->DirectBuffer.FlushItem(light.Index, data);
        }

        void LightManager::FlushPointLight(const PointLightComponent &light) {
            PointLightBufferData data;
            data.Position = light.Position;
            data.Color = light.Color;
            data.Constant = light.Constant;
            data.Linear = light.Linear;
            data.Quadratic = light.Quadratic;
            s_Storage->PointBuffer.FlushItem(light.Index, data);
        }

        void LightManager::FlushSpotLight(const SpotLightComponent &light) {
            SpotLightBufferData data;
            data.Position = light.Position;
            data.Direction = light.Direction;
            data.Color = light.Color;
            data.Cutoff = light.Cutoff;
            data.Outer = light.Outer;
            s_Storage->SpotBuffer.FlushItem(light.Index, data);
        }

        void LightManager::FlushDirectLights() {
            s_Storage->DirectBuffer.Flush();
        }

        void LightManager::FlushPointLights() {
            s_Storage->PointBuffer.Flush();
        }

        void LightManager::FlushSpotLights() {
            s_Storage->SpotBuffer.Flush();
        }

    }

}