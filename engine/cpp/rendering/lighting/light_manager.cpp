#include <rendering/lighting/light_manager.h>

namespace xpe {

    namespace render {

        Context* LightManager::s_Context;
        DirectLightBuffer LightManager::s_DirectLightBuffer;
        PointLightBuffer LightManager::s_PointLightBuffer;
        SpotLightBuffer LightManager::s_SpotLightBuffer;

        void LightManager::Init(Context* context) {
            LogInfo("LightManager::Init()");

            s_Context = context;
            s_DirectLightBuffer = DirectLightBuffer(context, K_DIRECT_LIGHT_COUNT);
            s_PointLightBuffer = PointLightBuffer(context, K_POINT_LIGHT_COUNT);
            s_SpotLightBuffer = SpotLightBuffer(context, K_SPOT_LIGHT_COUNT);

            LogInfo("LightManager initialized");
        }

        void LightManager::Free() {
            LogInfo("LightManager::Free()");

            s_DirectLightBuffer.Free();
            s_PointLightBuffer.Free();
            s_SpotLightBuffer.Free();
        }

        void LightManager::InitLight(DirectLight &light) {
            light.Data = s_DirectLightBuffer.GetItem(light.Index);
        }

        void LightManager::InitLight(PointLight &light) {
            light.Data = s_PointLightBuffer.GetItem(light.Index);
        }

        void LightManager::InitLight(SpotLight &light) {
            light.Data = s_SpotLightBuffer.GetItem(light.Index);
        }

        void LightManager::FreeLight(DirectLight &light) {
            light.Data = nullptr;
        }

        void LightManager::FreeLight(PointLight &light) {
            light.Data = nullptr;
        }

        void LightManager::FreeLight(SpotLight &light) {
            light.Data = nullptr;
        }

        vector<DirectLightBufferData>& LightManager::DirectList() {
            return s_DirectLightBuffer.GetList();
        }

        vector<PointLightBufferData>& LightManager::PointList() {
            return s_PointLightBuffer.GetList();
        }

        vector<SpotLightBufferData>& LightManager::SpotList() {
            return s_SpotLightBuffer.GetList();
        }

        DirectLightBuffer* LightManager::GetDirectBuffer() {
            return &s_DirectLightBuffer;
        }

        PointLightBuffer* LightManager::GetPointBuffer() {
            return &s_PointLightBuffer;
        }

        SpotLightBuffer* LightManager::GetSpotBuffer() {
            return &s_SpotLightBuffer;
        }

        void LightManager::UpdateLight(const DirectLight &light) {
            s_DirectLightBuffer.FlushItem(light.Index, *light.Data);
        }

        void LightManager::UpdateLight(const PointLight &light) {
            s_PointLightBuffer.FlushItem(light.Index, *light.Data);
        }

        void LightManager::UpdateLight(const SpotLight &light) {
            s_SpotLightBuffer.FlushItem(light.Index, *light.Data);
        }

    }

}