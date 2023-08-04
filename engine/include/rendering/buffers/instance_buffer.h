#pragma once

#include <rendering/buffers/structure_buffer.h>

namespace xpe {

    namespace render {

        struct ENGINE_API RenderInstance final
        {
            u32 TransformIndex = 0;
            u32 CameraIndex = 0;
            u32 MaterialIndex = 0;

            friend inline bool operator ==(const RenderInstance& instance1, const RenderInstance& instance2) {
                return instance1.TransformIndex == instance2.TransformIndex;
            }
        };

        struct ENGINE_API RenderInstance2d final
        {
            u32 TransformIndex = 0;
            u32 CameraIndex = 0;

            friend inline bool operator ==(const RenderInstance2d& instance1, const RenderInstance2d& instance2) {
                return instance1.TransformIndex == instance2.TransformIndex;
            }
        };

        struct ENGINE_API TextGlyphInstance final
        {
            u32 TransformIndex = 0;
            u32 CameraIndex = 0;
            f32 GlyphSize = 0;
            f32 Width = 0;
            f32 Height = 0;
            f32 Left = 0;
            f32 Top = 0;
            f32 AdvanceX = 0;
            f32 AdvanceY = 0;
            f32 AtlasXOffset = 0;
            f32 AtlasYOffset = 0;

            friend inline bool operator ==(const TextGlyphInstance& instance1, const TextGlyphInstance& instance2) {
                return instance1.TransformIndex == instance2.TransformIndex;
            }
        };

        class ENGINE_API InstanceBuffer : public StructureBuffer<RenderInstance> {

        public:
            InstanceBuffer() = default;

            InstanceBuffer(Context* context, usize count)
            : StructureBuffer<RenderInstance>(context, count, K_SLOT_INSTANCES, K_FALSE) {}

        public:
            void Bind();

        };

        class ENGINE_API InstanceBuffer2d : public StructureBuffer<RenderInstance2d> {

        public:
            InstanceBuffer2d() = default;

            InstanceBuffer2d(Context* context, usize count)
            : StructureBuffer<RenderInstance2d>(context, count, K_SLOT_INSTANCES2D, K_FALSE) {}

        public:
            void Bind();

        };

        class ENGINE_API TextGlyphInstanceBuffer : public StructureBuffer<TextGlyphInstance> {

        public:
            TextGlyphInstanceBuffer() = default;

            TextGlyphInstanceBuffer(Context* context, usize count)
            : StructureBuffer<TextGlyphInstance>(context, count, K_SLOT_TEXT_GLYPH_INSTANCES, K_FALSE) {}

        public:
            void Bind();

        };

    }

}