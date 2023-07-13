#include <rendering/transforming.h>

namespace xpe {

    namespace render {

        Context* TransformManager::s_Context = nullptr;
        TransformBuffer TransformManager::s_TransformBuffer;
        Transform2DBuffer TransformManager::s_Transform2DBuffer;

        void TransformManager::Init(Context* context) {
            s_Context = context;
            s_Transform2DBuffer = Transform2DBuffer(context, K_TRANSFORMS2D_SIZE);
            s_TransformBuffer = TransformBuffer(context, K_TRANSFORMS_SIZE);
        }

        void TransformManager::Free() {
            s_Transform2DBuffer.Free();
            s_TransformBuffer.Free();
        }

        TransformBuffer* TransformManager::GetBuffer() {
            return &s_TransformBuffer;
        }

        Transform2DBuffer* TransformManager::GetBuffer2D() {
            return &s_Transform2DBuffer;
        }

        void TransformManager::UpdateTransform(u32 index, const TransformComponent &transformComponent) {
            math::ModelMatrix modelMatrix;
            modelMatrix.Translation = transformComponent.Position;
            modelMatrix.Rotation = transformComponent.Rotation;
            modelMatrix.Scale = transformComponent.Scale;

            TransformData transformData;
            transformData.Matrix = ModelMatrixUpdate(modelMatrix);

            s_TransformBuffer.FlushItem(index, transformData);
        }

        void TransformManager::UpdateTransform2D(u32 index, const Transform2DComponent &transform2DComponent) {
            math::Model2dMatrix modelMatrix;
            modelMatrix.Translation = transform2DComponent.Position;
            modelMatrix.Rotation = transform2DComponent.Rotation;
            modelMatrix.Scale = transform2DComponent.Scale;

            Transform2DData transformData;
            transformData.Matrix = Model2dMatrixUpdate(modelMatrix);

            s_Transform2DBuffer.FlushItem(index, transformData);
        }

    }

}