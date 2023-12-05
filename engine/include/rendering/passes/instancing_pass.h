#pragma once

#include <rendering/passes/render_pass.h>
#include <rendering/buffers/instance_buffer.h>
#include <rendering/geometry/geometry.h>

namespace xpe {

    namespace render {

        class ENGINE_API cInstancingPass : public cRenderPass
        {

        public:
            cInstancingPass(eType type, const vector<sRenderPassBinding>& bindings);

        protected:
            void DrawInstanced(
                cScene* scene,
                const sGeometryInfo& geometryInfo,
                const std::function<void(EntityID entityId, sRenderInstance&)>& callback = [](EntityID, sRenderInstance&) {}
            );

            sInstanceBuffer m_InstanceBuffer;
        };

    }

}