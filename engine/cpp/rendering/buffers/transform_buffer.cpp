#include <rendering/buffers/transform_buffer.h>

namespace xpe {

    namespace render {

        void TransformBuffer::AddTransform(const Transform& transform, const glm::mat4x4& lightMatrix)
        {
            ModelMatrix modelMatrix;
            modelMatrix.Translation = transform.Position;
            modelMatrix.Rotation = transform.Rotation;
            modelMatrix.Scale = transform.Scale;

            TransformData transformData;
            transformData.ModelMatrix = ModelMatrixUpdate(modelMatrix);
            transformData.NormalMatrix = glm::transpose(glm::inverse(glm::mat3(transformData.ModelMatrix)));
            transformData.LightMatrix = lightMatrix;

            Add(transformData);
        }

    }

}