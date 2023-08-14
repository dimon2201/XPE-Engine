#include <rendering/buffers/transform_buffer.h>

namespace xpe {

    namespace render {

        void TransformBuffer::AddTransform(const Transform& transform)
        {
            ModelMatrix modelMatrix;
            modelMatrix.Translation = transform.Position;
            modelMatrix.Rotation = transform.Rotation;
            modelMatrix.Scale = transform.Scale;

            TransformData transformData;
            transformData.ModelMatrix = ModelMatrixUpdate(modelMatrix);
            transformData.NormalMatrix = glm::transpose(glm::inverse(glm::mat3(transformData.ModelMatrix)));

            Add(transformData);
        }

        void Transform2DBuffer::AddTransform(const Transform2D& transform)
        {
            Model2dMatrix modelMatrix;
            modelMatrix.Translation = transform.Position;
            modelMatrix.Rotation = transform.Rotation;
            modelMatrix.Scale = transform.Scale;

            Transform2DData transformData;
            transformData.ModelMatrix = Model2dMatrixUpdate(modelMatrix);

            Add(transformData);
        }

    }

}