#include <geometry/vertices.h>

namespace xpe {

    namespace math {

        VertexFormat Vertex2D::Format =
        {{
             Attributes::POS2,
             Attributes::UV
         }};

        VertexFormat Vertex3D::Format =
        {{
             Attributes::POS3,
             Attributes::UV,
             Attributes::NORMAL,
             Attributes::TANGENT
         }};

    }

}