#include <rendering/buffers/light_buffers.h>
#include <ecs/components.hpp>

namespace xpe {

    namespace render {

        void DirectLightBuffer::AddComponent(DirectLightComponent &component)
        {
            DirectLightBufferData data;
            data.Position = component.Position;
            data.Color = component.Color;
            Add(data);
        }

        void PointLightBuffer::AddComponent(PointLightComponent &component)
        {
            PointLightBufferData data;
            data.Position = component.Position;
            data.Color = component.Color;
            data.Constant = component.Constant;
            data.Linear = component.Linear;
            data.Quadratic = component.Quadratic;
            Add(data);
        }

        void SpotLightBuffer::AddComponent(SpotLightComponent &component)
        {
            SpotLightBufferData data;
            data.Position = component.Position;
            data.Direction = component.Direction;
            data.Color = component.Color;
            data.Cutoff = component.Cutoff;
            data.Outer = component.Outer;
            Add(data);
        }

    }

}