#pragma once

namespace xpe {

    namespace render {

        using namespace core;

        struct ENGINE_API sResource : public cObject
        {
            enum class eViewType
            {
                NONE,
                DEPTH_STENCIL,
                RENDER_TARGET,
                SRV,
                UAV
            };

            enum class eMapType
            {
                READ,
                WRITE,
                READ_WRITE,
                WRITE_NO_OVERWRITE,
                WRITE_DISCARD
            };

            void* Instance = nullptr;
            void* ViewInstance = nullptr;
            eViewType ViewType = eViewType::NONE;
        };

        enum class ePrimitiveTopology
        {
            TRIANGLE_LIST,
            TRIANGLE_STRIP,
            POINT_LIST,
            LINE_LIST,
            LINE_STRIP,

            DEFAULT = TRIANGLE_LIST
        };

        struct ENGINE_API sBuffer : public sResource
        {
            enum class eType
            {
                NONE,
                VERTEX,
                INDEX,
                ITEM,
                LIST,

                CONSTANT = ITEM,
                STRUCTURED = LIST,
            };

            enum class eSubType
            {
                NONE,
                RAW,     // combined with VERTEX/INDEX/LIST buffer
                APPEND,  // combined with LIST/ITEM buffer
                CONSUME  // combined with LIST/ITEM buffer
            };

            enum class eUsage
            {
                DEFAULT,
                STATIC,
                DYNAMIC,
                STAGING
            };

            eType Type = eType::NONE;
            eSubType SubType = eSubType::NONE;
            eUsage Usage = eUsage::DYNAMIC;
            usize StructureSize = 0;
            u32 NumElements = 0;
            u32 Slot = 0;
            void* InitialData = nullptr;

            [[nodiscard]] inline usize GetByteSize() const
            {
                return NumElements * StructureSize;
            }
        };

        struct ENGINE_API sBlob : public cObject
        {
            void* Instance = nullptr;
            void* ByteCode = nullptr;
            usize ByteCodeSize = 0;
        };

    }

}