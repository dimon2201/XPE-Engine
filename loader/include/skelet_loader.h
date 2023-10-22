#pragma once

#include <build.h>
#include <assimp_types.h>

#include <anim/storages/skelet_storage.h>

namespace xpe {

    namespace res {

        using namespace core;
        using namespace anim;
        using namespace math;

        class LOADER_API SkeletLoader : public Object
        {

        public:
            SkeletLoader(SkeletStorage* skeletStorage) : m_Storage(skeletStorage) {}

            Ref<Skeleton> Load(const char* filepath, const vector<eLoadOption>& options = {
                    eLoadOption::TRIANGULATE
            });

        private:
            SkeletStorage* m_Storage;
        };

    }

}