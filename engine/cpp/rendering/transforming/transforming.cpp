#include <rendering/transforming/transforming.h>
#include <ecs/components.hpp>

namespace xpe {

    namespace render {

        using namespace ecs;

        Context* TransformManager::s_Context = nullptr;
        TransformStorage* TransformManager::s_Storage = nullptr;

        TransformStorage::TransformStorage(Context* context) {
            Buffer2D = Transform2DBuffer(context, 0);
            Buffer2D.Reserve(K_TRANSFORMS2D_SIZE);

            Buffer = TransformBuffer(context, 0);
            Buffer.Reserve(K_TRANSFORMS_SIZE);
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

        void TransformManager::AddTransform(TransformComponent &transformComponent) {
            math::ModelMatrix modelMatrix;
            modelMatrix.Translation = transformComponent.Position;
            modelMatrix.Rotation = transformComponent.Rotation;
            modelMatrix.Scale = transformComponent.Scale;

            TransformData transformData;
            transformData.ModelMatrix = ModelMatrixUpdate(modelMatrix);
            transformData.NormalMatrix = glm::transpose(glm::inverse(glm::mat3(transformData.ModelMatrix)));

            transformComponent.Index = s_Storage->Buffer.Add(transformData);
        }

        void TransformManager::AddTransform2D(Transform2DComponent &transformComponent) {
            math::Model2dMatrix modelMatrix;
            modelMatrix.Translation = transformComponent.Position;
            modelMatrix.Rotation = transformComponent.Rotation;
            modelMatrix.Scale = transformComponent.Scale;

            Transform2DData transformData;
            transformData.ModelMatrix = Model2dMatrixUpdate(modelMatrix);

            transformComponent.Index = s_Storage->Buffer2D.Add(transformData);
        }

        void TransformManager::RemoveTransform(TransformComponent &transformComponent) {
            s_Storage->Buffer.RemoveAt(transformComponent.Index);
        }

        void TransformManager::RemoveTransform2D(Transform2DComponent &transformComponent) {
            s_Storage->Buffer2D.RemoveAt(transformComponent.Index);
        }

        void TransformManager::FlushTransform(const TransformComponent &transformComponent) {
            math::ModelMatrix modelMatrix;
            modelMatrix.Translation = transformComponent.Position;
            modelMatrix.Rotation = transformComponent.Rotation;
            modelMatrix.Scale = transformComponent.Scale;

            TransformData transformData;
            transformData.ModelMatrix = ModelMatrixUpdate(modelMatrix);
            transformData.NormalMatrix = glm::transpose(glm::inverse(glm::mat3(transformData.ModelMatrix)));

            s_Storage->Buffer.FlushItem(transformComponent.Index, transformData);
        }

        void TransformManager::FlushTransform2D(const Transform2DComponent &transform2DComponent) {
            math::Model2dMatrix modelMatrix;
            modelMatrix.Translation = transform2DComponent.Position;
            modelMatrix.Rotation = transform2DComponent.Rotation;
            modelMatrix.Scale = transform2DComponent.Scale;

            Transform2DData transformData;
            transformData.ModelMatrix = Model2dMatrixUpdate(modelMatrix);

            s_Storage->Buffer2D.FlushItem(transform2DComponent.Index, transformData);
        }

        void TransformManager::FlushTransforms() {
            s_Storage->Buffer.Flush();
        }

        void TransformManager::FlushTransforms2D() {
            s_Storage->Buffer2D.Flush();
        }

        void TransformManager::ClearTransforms() {
            s_Storage->Buffer.Clear();
        }

        void TransformManager::ClearTransforms2D() {
            s_Storage->Buffer2D.Clear();
        }
    }

}