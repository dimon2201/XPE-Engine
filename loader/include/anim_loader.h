#pragma once

#include <build.h>
#include <assimp_types.h>

#include <anim/storages/anim_storage.h>

namespace xpe {

    namespace res {

        using namespace core;
        using namespace anim;
        using namespace math;

        class LOADER_API AnimLoader : public Object
        {

        public:
            AnimLoader(AnimStorage* animStorage) : m_Storage(animStorage) {}

            Ref<Animation> Load(const char* filepath, const vector<eLoadOption>& options = {
                    eLoadOption::TRIANGULATE
            });

        private:
            AnimStorage* m_Storage;
        };

    }

}