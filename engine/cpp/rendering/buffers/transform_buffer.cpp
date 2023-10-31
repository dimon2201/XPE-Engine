#include <rendering/buffers/transform_buffer.h>
#include <math/math_manager.h>

namespace xpe {

    namespace render {

        void sTransformBuffer::AddTransform(const sTransform& transform, const glm::mat4x4& lightMatrix)
        {
            sModelMatrix modelMatrix;
            modelMatrix.Translation = transform.Position;
            modelMatrix.Rotation = transform.Rotation;
            modelMatrix.Scale = transform.Scale;

            sTransformData transformData;
            transformData.ModelMatrix = MathManager::UpdateModelMatrix(modelMatrix);
            transformData.NormalMatrix = glm::transpose(glm::inverse(glm::mat3(transformData.ModelMatrix)));
            transformData.LightMatrix = lightMatrix;

            Add(transformData);
        }

    }

}