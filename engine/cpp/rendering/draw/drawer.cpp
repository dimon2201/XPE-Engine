#include <rendering/draw/drawer.h>
#include <rendering/buffers/camera_buffer.h>

namespace xpe {

    namespace render {

        Drawer::Drawer(CameraBuffer* cameraBuffer, Shader* shader)
        {
            m_Pipeline = new Pipeline();
            m_Pipeline->VSBuffers.emplace_back(cameraBuffer);
            m_Pipeline->Shader = shader;
            m_Pipeline->Blending.Targets[0].Enable = true;
            m_Pipeline->Blending.Targets[0].Src = eBlend::SRC_ALPHA;
            m_Pipeline->Blending.Targets[0].Dest = eBlend::INV_SRC_ALPHA;
        }

        Drawer::~Drawer()
        {
            context::FreePipeline(*m_Pipeline);
            delete m_Pipeline;
        }

        void Drawer::Begin(RenderTarget *renderTarget)
        {
            m_Pipeline->RenderTarget = renderTarget;
            context::BindPipeline(*m_Pipeline);
        }

        void Drawer::End()
        {
            context::UnbindPipeline(*m_Pipeline);
        }

        void Drawer::Init()
        {
            context::CreatePipeline(*m_Pipeline);
        }

    }

}