#include <geometry/geometry.h>

namespace xpe {

    namespace math {

        void IndexArray::Init(usize count) {
            Data.resize(count);
        }

        void IndexArray::Free() {
            Data.clear();
        }

        void IndexArray::CopyFrom(IndexArray* src) {
            memcpy(Data.data(), src->Data.data(), src->Size());
        }

        void IndexArray::CopyFrom(u32* indices, usize count) {
            memcpy(Data.data(), indices, count * sizeof(u32));
        }

        void IndexArray::Reserve(usize count) {
            Data.reserve(count);
        }

    }

}