#include <rendering/transforming.h>

namespace xpe {

    namespace render {

        Context* TransformManager::s_Context = nullptr;
        TransformStorage* TransformManager::s_Storage = nullptr;

        TransformStorage::TransformStorage(Context* context) {
            Buffer2D = Transform2DBuffer(context, K_TRANSFORMS2D_SIZE);
            Buffer = TransformBuffer(context, K_TRANSFORMS_SIZE);
        }

        TransformStorage::~TransformStorage() {
            Buffer2D.Free();
            Buffer.Free();
        }

        void TransformManager::Init(Context* context) {
            LogInfo("TransformManager::Init()");

            s_Context = context;
            s_Storage = new TransformStorage(context);

            LogInfo("TransformManager initialized");
        }

        void TransformManager::Free() {
            LogInfo("TransformManager::Free()");
            delete s_Storage;
        }

        TransformBuffer* TransformManager::GetBuffer() {
            return &s_Storage->Buffer;
        }

        Transform2DBuffer* TransformManager::GetBuffer2D() {
            return &s_Storage->Buffer2D;
        }

        void TransformManager::UpdateTransform(u32 index, const TransformComponent &transformComponent) {
            math::ModelMatrix modelMatrix;
            modelMatrix.Translation = transformComponent.Position;
            modelMatrix.Rotation = transformComponent.Rotation;
            modelMatrix.Scale = transformComponent.Scale;

            TransformData transformData;
            transformData.Matrix = ModelMatrixUpdate(modelMatrix);

            s_Storage->Buffer.FlushItem(index, transformData);
        }

        void TransformManager::UpdateTransform2D(u32 index, const Transform2DComponent &transform2DComponent) {
            math::Model2dMatrix modelMatrix;
            modelMatrix.Translation = transform2DComponent.Position;
            modelMatrix.Rotation = transform2DComponent.Rotation;
            modelMatrix.Scale = transform2DComponent.Scale;

            Transform2DData transformData;
            transformData.Matrix = Model2dMatrixUpdate(modelMatrix);

            s_Storage->Buffer2D.FlushItem(index, transformData);
        }

    }

}