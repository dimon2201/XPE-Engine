#include <rendering/draw/drawer.h>
#include <rendering/monitor.h>
#include <rendering/buffers/camera_buffer.h>

namespace xpe {

    namespace render {

        Drawer::Drawer(CameraBuffer* cameraBuffer, Shader* shader, RenderTarget* renderTarget)
        {
            m_Pipeline = new Pipeline();
            m_Pipeline->VSBuffers.emplace_back(cameraBuffer);
            m_Pipeline->PSBuffers.emplace_back(Monitor::Get().GetBuffer());
            m_Pipeline->Shader = shader;
            m_Pipeline->RenderTarget = renderTarget;
            m_Pipeline->Blending.Targets[0].Enable = true;
            m_Pipeline->Blending.Targets[0].Src = eBlend::SRC_ALPHA;
            m_Pipeline->Blending.Targets[0].Dest = eBlend::INV_SRC_ALPHA;
        }

        Drawer::~Drawer()
        {
            context::FreePipeline(*m_Pipeline);
            delete m_Pipeline;
        }

        void Drawer::Begin()
        {
            context::BindPipeline(*m_Pipeline);
        }

        void Drawer::End()
        {
            context::UnbindPipeline(*m_Pipeline);
        }

        RenderTarget* Drawer::GetRenderTarget()
        {
            return m_Pipeline->RenderTarget;
        }

        void Drawer::Init()
        {
            context::CreatePipeline(*m_Pipeline);
        }

    }

}