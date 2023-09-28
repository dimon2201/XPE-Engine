#include <rendering/draw/drawer.h>
#include <rendering/monitor.h>

namespace xpe {

    namespace render {

        Drawer::Drawer(
                Shader* shader, RenderTarget* renderTarget,
                const vector<Buffer*> &VSBuffers,
                const vector<Buffer*> &PSBuffers
        ) {
            m_Pipeline = new Pipeline();
            m_Pipeline->Shader = shader;
            m_Pipeline->RenderTarget = renderTarget;
            m_Pipeline->VSBuffers = VSBuffers;
            m_Pipeline->PSBuffers = PSBuffers;
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

        void Drawer::Init()
        {
            context::CreatePipeline(*m_Pipeline);
        }

    }

}