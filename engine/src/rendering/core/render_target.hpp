#pragma once

#include <rendering/core/core.hpp>

namespace xpe {

    namespace render {

        struct ENGINE_API cRenderTarget : public cObject
        {

        public:
            cRenderTarget(const vector<cTexture*>& colors);
            cRenderTarget(const cTexture* depthStencil);
            cRenderTarget(const vector<cTexture*>& colors, const cTexture* depthStencil);
            cRenderTarget(const vector<void*>& colorViews);

            ~cRenderTarget();

            void Resize(s32 width, s32 height);
            void ResizeColors(s32 width, s32 height);

            void Bind();
            void BindColor(u32 index);
            void BindDepth();
            void Unbind();

            void WindowFrameResized(s32 width, s32 height);

            void Clear();

            void SetResizable(bool resizable);
            [[nodiscard]] inline bool IsResizable() const { return m_Resizable; }

            inline vector<void*>& GetColorViews() { return m_ColorViews; }
            inline void* GetDepthStencilView() { return m_DepthStencilView; }
            inline void** GetDepthStencilViewPtr() { return &m_DepthStencilView; }
            inline vector<cTexture*>& GetColors() { return m_Colors; }
            inline cTexture* GetDepthStencil() { return m_DepthStencil; }
            inline cTexture** GetDepthStencilPtr() { return &m_DepthStencil; }
            inline vector<glm::vec4>& GetClearColors() { return m_ClearColors; }
            inline float GetClearDepth() { return m_ClearDepth; }

            inline void SetColor(s32 bufferIndex, cTexture* texture) { m_Colors[bufferIndex] = texture; }
            inline void SetDepthStencil(cTexture* texture) { m_DepthStencil = texture; }
            inline void SetClearColors(s32 bufferIndex, const glm::vec4& value) { m_ClearColors[bufferIndex] = value; }
            inline void SetClearDepth(float value) { m_ClearDepth = value; }

        private:
            void Init();

            bool m_Resizable = false;
            vector<void*> m_ColorViews;
            void* m_DepthStencilView = nullptr;
            vector<cTexture*> m_Colors;
            cTexture* m_DepthStencil = nullptr;
            vector<glm::vec4> m_ClearColors;
            float m_ClearDepth = K_DEPTH_INVALID;

        };

    }

}