#include <geometry/geometry.h>

namespace xpe {

    namespace math {

        void IndexArray::Init(usize count)
        {
            Data.resize(count);
        }

        void IndexArray::Reserve(usize count)
        {
            Data.reserve(count);
        }

    }

}