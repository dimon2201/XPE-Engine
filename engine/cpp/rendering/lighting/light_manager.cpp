#include <rendering/lighting/light_manager.h>

namespace xpe {

    namespace render {

        Context* LightManager::s_Context = nullptr;
        LightStorage* LightManager::s_Storage = nullptr;

        LightStorage::~LightStorage() {
            DirectBuffer.Free();
            PointBuffer.Free();
            SpotBuffer.Free();
        }

        void LightManager::Init(Context* context) {
            LogInfo("LightManager::Init()");

            s_Context = context;
            s_Storage = new LightStorage();
            s_Storage->DirectBuffer = DirectLightBuffer(context, K_DIRECT_LIGHT_COUNT);
            s_Storage->PointBuffer = PointLightBuffer(context, K_POINT_LIGHT_COUNT);
            s_Storage->SpotBuffer = SpotLightBuffer(context, K_SPOT_LIGHT_COUNT);

            LogInfo("LightManager initialized");
        }

        void LightManager::Free() {
            LogInfo("LightManager::Free()");
            delete s_Storage;
        }

        void LightManager::InitLight(DirectLight &light) {
            light.Data = s_Storage->DirectBuffer.GetItem(light.Index);
        }

        void LightManager::InitLight(PointLight &light) {
            light.Data = s_Storage->PointBuffer.GetItem(light.Index);
        }

        void LightManager::InitLight(SpotLight &light) {
            light.Data = s_Storage->SpotBuffer.GetItem(light.Index);
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
            return s_Storage->DirectBuffer.GetList();
        }

        vector<PointLightBufferData>& LightManager::PointList() {
            return s_Storage->PointBuffer.GetList();
        }

        vector<SpotLightBufferData>& LightManager::SpotList() {
            return s_Storage->SpotBuffer.GetList();
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

        void LightManager::UpdateLight(const DirectLight &light) {
            s_Storage->DirectBuffer.FlushItem(light.Index, *light.Data);
        }

        void LightManager::UpdateLight(const PointLight &light) {
            s_Storage->PointBuffer.FlushItem(light.Index, *light.Data);
        }

        void LightManager::UpdateLight(const SpotLight &light) {
            s_Storage->SpotBuffer.FlushItem(light.Index, *light.Data);
        }

    }

}