#pragma once

#include <stl/string.h>
#include <stl/vector.h>

#include <pugixml.hpp>

using xml = pugi::xml_node;

#define XML_EXPAND( x ) x
#define XML_GET_MACRO(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, NAME,...) NAME
#define XML_PASTE(...) XML_EXPAND(XML_GET_MACRO(__VA_ARGS__, \
        XML_PASTE64, \
        XML_PASTE63, \
        XML_PASTE62, \
        XML_PASTE61, \
        XML_PASTE60, \
        XML_PASTE59, \
        XML_PASTE58, \
        XML_PASTE57, \
        XML_PASTE56, \
        XML_PASTE55, \
        XML_PASTE54, \
        XML_PASTE53, \
        XML_PASTE52, \
        XML_PASTE51, \
        XML_PASTE50, \
        XML_PASTE49, \
        XML_PASTE48, \
        XML_PASTE47, \
        XML_PASTE46, \
        XML_PASTE45, \
        XML_PASTE44, \
        XML_PASTE43, \
        XML_PASTE42, \
        XML_PASTE41, \
        XML_PASTE40, \
        XML_PASTE39, \
        XML_PASTE38, \
        XML_PASTE37, \
        XML_PASTE36, \
        XML_PASTE35, \
        XML_PASTE34, \
        XML_PASTE33, \
        XML_PASTE32, \
        XML_PASTE31, \
        XML_PASTE30, \
        XML_PASTE29, \
        XML_PASTE28, \
        XML_PASTE27, \
        XML_PASTE26, \
        XML_PASTE25, \
        XML_PASTE24, \
        XML_PASTE23, \
        XML_PASTE22, \
        XML_PASTE21, \
        XML_PASTE20, \
        XML_PASTE19, \
        XML_PASTE18, \
        XML_PASTE17, \
        XML_PASTE16, \
        XML_PASTE15, \
        XML_PASTE14, \
        XML_PASTE13, \
        XML_PASTE12, \
        XML_PASTE11, \
        XML_PASTE10, \
        XML_PASTE9, \
        XML_PASTE8, \
        XML_PASTE7, \
        XML_PASTE6, \
        XML_PASTE5, \
        XML_PASTE4, \
        XML_PASTE3, \
        XML_PASTE2, \
        XML_PASTE1)(__VA_ARGS__))
#define XML_PASTE2(func, v1) func(v1)
#define XML_PASTE3(func, v1, v2) XML_PASTE2(func, v1) XML_PASTE2(func, v2)
#define XML_PASTE4(func, v1, v2, v3) XML_PASTE2(func, v1) XML_PASTE3(func, v2, v3)
#define XML_PASTE5(func, v1, v2, v3, v4) XML_PASTE2(func, v1) XML_PASTE4(func, v2, v3, v4)
#define XML_PASTE6(func, v1, v2, v3, v4, v5) XML_PASTE2(func, v1) XML_PASTE5(func, v2, v3, v4, v5)
#define XML_PASTE7(func, v1, v2, v3, v4, v5, v6) XML_PASTE2(func, v1) XML_PASTE6(func, v2, v3, v4, v5, v6)
#define XML_PASTE8(func, v1, v2, v3, v4, v5, v6, v7) XML_PASTE2(func, v1) XML_PASTE7(func, v2, v3, v4, v5, v6, v7)
#define XML_PASTE9(func, v1, v2, v3, v4, v5, v6, v7, v8) XML_PASTE2(func, v1) XML_PASTE8(func, v2, v3, v4, v5, v6, v7, v8)
#define XML_PASTE10(func, v1, v2, v3, v4, v5, v6, v7, v8, v9) XML_PASTE2(func, v1) XML_PASTE9(func, v2, v3, v4, v5, v6, v7, v8, v9)
#define XML_PASTE11(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10) XML_PASTE2(func, v1) XML_PASTE10(func, v2, v3, v4, v5, v6, v7, v8, v9, v10)
#define XML_PASTE12(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11) XML_PASTE2(func, v1) XML_PASTE11(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11)
#define XML_PASTE13(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12) XML_PASTE2(func, v1) XML_PASTE12(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12)
#define XML_PASTE14(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13) XML_PASTE2(func, v1) XML_PASTE13(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13)
#define XML_PASTE15(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14) XML_PASTE2(func, v1) XML_PASTE14(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14)
#define XML_PASTE16(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15) XML_PASTE2(func, v1) XML_PASTE15(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15)
#define XML_PASTE17(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16) XML_PASTE2(func, v1) XML_PASTE16(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16)
#define XML_PASTE18(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17) XML_PASTE2(func, v1) XML_PASTE17(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17)
#define XML_PASTE19(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18) XML_PASTE2(func, v1) XML_PASTE18(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18)
#define XML_PASTE20(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19) XML_PASTE2(func, v1) XML_PASTE19(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19)
#define XML_PASTE21(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20) XML_PASTE2(func, v1) XML_PASTE20(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20)
#define XML_PASTE22(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21) XML_PASTE2(func, v1) XML_PASTE21(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21)
#define XML_PASTE23(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22) XML_PASTE2(func, v1) XML_PASTE22(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22)
#define XML_PASTE24(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23) XML_PASTE2(func, v1) XML_PASTE23(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23)
#define XML_PASTE25(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24) XML_PASTE2(func, v1) XML_PASTE24(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24)
#define XML_PASTE26(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25) XML_PASTE2(func, v1) XML_PASTE25(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25)
#define XML_PASTE27(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26) NXML_PASTE2(func, v1) XML_PASTE26(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26)
#define XML_PASTE28(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27) XML_PASTE2(func, v1) XML_PASTE27(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27)
#define XML_PASTE29(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28) XML_PASTE2(func, v1) XML_PASTE28(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28)
#define XML_PASTE30(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29) XML_PASTE2(func, v1) XML_PASTE29(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29)
#define XML_PASTE31(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30) XML_PASTE2(func, v1) XML_PASTE30(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30)
#define XML_PASTE32(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31) XML_PASTE2(func, v1) XML_PASTE31(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31)
#define XML_PASTE33(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32) XML_PASTE2(func, v1) XML_PASTE32(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32)
#define XML_PASTE34(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33) XML_PASTE2(func, v1) XML_PASTE33(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33)
#define XML_PASTE35(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34) XML_PASTE2(func, v1) XML_PASTE34(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34)
#define XML_PASTE36(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35) XML_PASTE2(func, v1) XML_PASTE35(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35)
#define XML_PASTE37(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36) XML_PASTE2(func, v1) XML_PASTE36(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36)
#define XML_PASTE38(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37) XML_PASTE2(func, v1) XML_PASTE37(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37)
#define XML_PASTE39(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38) XML_PASTE2(func, v1) XML_PASTE38(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38)
#define XML_PASTE40(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39) XML_PASTE2(func, v1) XML_PASTE39(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39)
#define XML_PASTE41(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40) XML_PASTE2(func, v1) XML_PASTE40(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40)
#define XML_PASTE42(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41) XML_PASTE2(func, v1) XML_PASTE41(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41)
#define XML_PASTE43(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42) XML_PASTE2(func, v1) XML_PASTE42(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42)
#define XML_PASTE44(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43) XML_PASTE2(func, v1) XML_PASTE43(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43)
#define XML_PASTE45(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44) XML_PASTE2(func, v1) XML_PASTE44(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44)
#define XML_PASTE46(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45) XML_PASTE2(func, v1) XML_PASTE45(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45)
#define XML_PASTE47(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46) XML_PASTE2(func, v1) XML_PASTE46(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46)
#define XML_PASTE48(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47) XML_PASTE2(func, v1) XML_PASTE47(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47)
#define XML_PASTE49(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48) XML_PASTE2(func, v1) XML_PASTE48(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48)
#define XML_PASTE50(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49) XML_PASTE2(func, v1) XML_PASTE49(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49)
#define XML_PASTE51(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50) XML_PASTE2(func, v1) XML_PASTE50(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50)
#define XML_PASTE52(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51) XML_PASTE2(func, v1) XML_PASTE51(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51)
#define XML_PASTE53(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52) XML_PASTE2(func, v1) XML_PASTE52(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52)
#define XML_PASTE54(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53) XML_PASTE2(func, v1) XML_PASTE53(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53)
#define XML_PASTE55(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54) XML_PASTE2(func, v1) XML_PASTE54(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54)
#define XML_PASTE56(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55) XML_PASTE2(func, v1) XML_PASTE55(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55)
#define XML_PASTE57(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56) XML_PASTE2(func, v1) XML_PASTE56(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56)
#define XML_PASTE58(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56, v57) XML_PASTE2(func, v1) XML_PASTE57(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56, v57)
#define XML_PASTE59(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56, v57, v58) XML_PASTE2(func, v1) XML_PASTE58(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56, v57, v58)
#define XML_PASTE60(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56, v57, v58, v59) XML_PASTE2(func, v1) XML_PASTE59(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56, v57, v58, v59)
#define XML_PASTE61(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56, v57, v58, v59, v60) XML_PASTE2(func, v1) XML_PASTE60(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56, v57, v58, v59, v60)
#define XML_PASTE62(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56, v57, v58, v59, v60, v61) XML_PASTE2(func, v1) XML_PASTE61(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56, v57, v58, v59, v60, v61)
#define XML_PASTE63(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56, v57, v58, v59, v60, v61, v62) XML_PASTE2(func, v1) XML_PASTE62(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56, v57, v58, v59, v60, v61, v62)
#define XML_PASTE64(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56, v57, v58, v59, v60, v61, v62, v63) XML_PASTE2(func, v1) XML_PASTE63(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56, v57, v58, v59, v60, v61, v62, v63)

#define XML_SET_VALUE(v1) xpe::res::SetXmlValue(root, #v1, value.v1);
#define XML_GET_VALUE(v1) xpe::res::GetXmlValue(root, #v1, value.v1);

#define Xml(type, ...)                          \
                                                \
inline void SetXmlValue(xml& root, const char* name, type& value) { \
    XML_EXPAND(XML_PASTE(XML_SET_VALUE, __VA_ARGS__))\
}                                               \
                                                \
inline void GetXmlValue(xml& root, const char* name, type& value) { \
    XML_EXPAND(XML_PASTE(XML_GET_VALUE, __VA_ARGS__))\
}

#define XML_TO(v1) SetXmlValue(child, #v1, v1);
#define XML_FROM(v1) GetXmlValue(child, #v1, v1);

#define XmlClass(name, ...)                      \
                                                 \
xml ToXml(xml& root) override {                 \
    auto child = root.append_child(name);         \
    XML_EXPAND(XML_PASTE(XML_TO, __VA_ARGS__))   \
    return child;                                                 \
}                                                \
                                                 \
xml FromXml(xml& root) override {               \
    auto child = root.child(name);                     \
    XML_EXPAND(XML_PASTE(XML_FROM, __VA_ARGS__)) \
    return child;                                                 \
}                                                \

#define XmlEnum(Enum, ...) \
                           \
    inline void SetXmlValue(xml& root, const char* name, Enum& e)                                  \
    {                                                                                           \
        static_assert(std::is_enum<Enum>::value, #Enum " must be an enum!");          \
        static const std::pair<Enum, std::string> m[] = __VA_ARGS__;                     \
        auto it = std::find_if(std::begin(m), std::end(m),                                      \
                               [e](const std::pair<Enum, std::string>& pair) -> bool  \
        {                                                                                       \
            return pair.first == e;                                                          \
        });                \
        std::string v = ((it != std::end(m)) ? it : std::begin(m))->second;\
        xpe::res::SetXmlValue(root, name, v);                                              \
    }                                                                                            \
                                                                                                \
    inline void GetXmlValue(xml& root, const char* name, Enum& e)                                 \
    {                                                                                           \
        static_assert(std::is_enum<Enum>::value, #Enum " must be an enum!");          \
        static const std::pair<Enum, std::string> m[] = __VA_ARGS__;                     \
        std::string v;                   \
        xpe::res::GetXmlValue(root, name, v);                                       \
        auto it = std::find_if(std::begin(m), std::end(m),                                      \
                               [&v](const std::pair<Enum, std::string>& pair) -> bool \
        {                                                                                       \
            return pair.second == v;                                                         \
        });                \
        e = ((it != std::end(m)) ? it : std::begin(m))->first;                               \
    }


namespace xpe {

    namespace res {

        class ENGINE_API cXml
        {

        public:
            xpe::core::vector<cXml*> XmlChildren;

            virtual xml ToXml(xml& root) = 0;
            virtual xml FromXml(xml& root) = 0;

            bool LoadFile(const char* filepath);
            bool SaveFile(const char* filepath);

        protected:
            pugi::xml_document m_Doc;

        private:
            void FromXmlChildren(xml root, xpe::core::vector<cXml*>& children);
            void ToXmlChildren(xml root, xpe::core::vector<cXml*>& children);
        };

        // Primitives
        ENGINE_API void SetXmlValue(xml& root, const char* name, bool& value);
        ENGINE_API void SetXmlValue(xml& root, const char* name, int& value);
        ENGINE_API void SetXmlValue(xml& root, const char* name, u32& value);
        ENGINE_API void SetXmlValue(xml& root, const char* name, float& value);
        ENGINE_API void SetXmlValue(xml& root, const char* name, double& value);
        ENGINE_API void SetXmlValue(xml& root, const char* name, u64& value);
        ENGINE_API void SetXmlValue(xml& root, const char* name, s64& value);

        // STL
        ENGINE_API void SetXmlValue(xml& root, const char* name, string& value);
        ENGINE_API void SetXmlValue(xml& root, const char* name, std::string& value);

        // GLM
        ENGINE_API void SetXmlValue(xml& root, const char* name, glm::vec2& value);
        ENGINE_API void SetXmlValue(xml& root, const char* name, glm::vec3& value);
        ENGINE_API void SetXmlValue(xml& root, const char* name, glm::vec4& value);

        // Primitives
        ENGINE_API void GetXmlValue(xml& root, const char* name, bool& value);
        ENGINE_API void GetXmlValue(xml& root, const char* name, int& value);
        ENGINE_API void GetXmlValue(xml& root, const char* name, u32& value);
        ENGINE_API void GetXmlValue(xml& root, const char* name, float& value);
        ENGINE_API void GetXmlValue(xml& root, const char* name, double& value);
        ENGINE_API void GetXmlValue(xml& root, const char* name, u64& value);
        ENGINE_API void GetXmlValue(xml& root, const char* name, s64& value);

        // STL
        ENGINE_API void GetXmlValue(xml& root, const char* name, string& value);
        ENGINE_API void GetXmlValue(xml& root, const char* name, std::string& value);

        // GLM
        ENGINE_API void GetXmlValue(xml& root, const char* name, glm::vec2& value);
        ENGINE_API void GetXmlValue(xml& root, const char* name, glm::vec3& value);
        ENGINE_API void GetXmlValue(xml& root, const char* name, glm::vec4& value);

    }

}

// XML for KeyCodes
namespace xpe::core
{

    XmlEnum(eKey, {

        { eKey::None, "None" },
        { eKey::Space, "Space" },
        { eKey::Apostrophe, "Apostrophe" },
        { eKey::Comma, "Comma" },
        { eKey::Minus, "Minus" },
        { eKey::Period, "Period" },
        { eKey::Slash, "Slash" },

        { eKey::D0, "D0" },
        { eKey::D1, "D1" },
        { eKey::D2, "D2" },
        { eKey::D3, "D3" },
        { eKey::D4, "D4" },
        { eKey::D5, "D5" },
        { eKey::D6, "D6" },
        { eKey::D7, "D7" },
        { eKey::D8, "D8" },
        { eKey::D9, "D9" },

        { eKey::Semicolon, "Semicolon" },
        { eKey::Equal, "Equal" },

        { eKey::A, "A" },
        { eKey::B, "B" },
        { eKey::C, "C" },
        { eKey::D, "D" },
        { eKey::E, "E" },
        { eKey::F, "F" },
        { eKey::G, "G" },
        { eKey::H, "H" },
        { eKey::I, "I" },
        { eKey::J, "J" },
        { eKey::K, "K" },
        { eKey::L, "L" },
        { eKey::M, "M" },
        { eKey::N, "N" },
        { eKey::O, "O" },
        { eKey::P, "P" },
        { eKey::Q, "Q" },
        { eKey::R, "R" },
        { eKey::S, "S" },
        { eKey::T, "T" },
        { eKey::U, "U" },
        { eKey::V, "V" },
        { eKey::W, "W" },
        { eKey::X, "X" },
        { eKey::Y, "Y" },
        { eKey::Z, "Z" },

        { eKey::LeftBracket, "LeftBracket" },
        { eKey::Backslash, "Backslash" },
        { eKey::RightBracket, "RightBracket" },
        { eKey::GraveAccent, "GraveAccent" },

        { eKey::World1, "World1" },
        { eKey::World2, "World2" },

        { eKey::Esc, "Esc" },
        { eKey::Enter, "Enter" },
        { eKey::Tab, "Tab" },
        { eKey::Backspace, "Backspace" },

        { eKey::Insert, "Insert" },
        { eKey::Delete, "Delete" },

        { eKey::Right, "Right" },
        { eKey::Left, "Left" },
        { eKey::Down, "Down" },
        { eKey::Up, "Up" },

        { eKey::PageUp, "PageUp" },
        { eKey::PageDown, "PageDown" },
        { eKey::Home, "Home" },
        { eKey::End, "End" },

        { eKey::CapsLock, "CapsLock" },
        { eKey::ScrollLock, "ScrollLock" },
        { eKey::NumLock, "NumLock" },

        { eKey::PrintScreen, "PrintScreen" },
        { eKey::Pause, "Pause" },

        { eKey::F1, "F1" },
        { eKey::F2, "F2" },
        { eKey::F3, "F3" },
        { eKey::F4, "F4" },
        { eKey::F5, "F5" },
        { eKey::F6, "F6" },
        { eKey::F7, "F7" },
        { eKey::F8, "F8" },
        { eKey::F9, "F9" },
        { eKey::F10, "F10" },
        { eKey::F11, "F11" },
        { eKey::F12, "F12" },
        { eKey::F13, "F13" },
        { eKey::F14, "F14" },
        { eKey::F15, "F15" },
        { eKey::F16, "F16" },
        { eKey::F17, "F17" },
        { eKey::F18, "F18" },
        { eKey::F19, "F19" },
        { eKey::F20, "F20" },
        { eKey::F21, "F21" },
        { eKey::F22, "F22" },
        { eKey::F23, "F23" },
        { eKey::F24, "F24" },
        { eKey::F25, "F25" },

        { eKey::KP0, "KP0" },
        { eKey::KP1, "KP1" },
        { eKey::KP2, "KP2" },
        { eKey::KP3, "KP3" },
        { eKey::KP4, "KP4" },
        { eKey::KP5, "KP5" },
        { eKey::KP6, "KP6" },
        { eKey::KP7, "KP7" },
        { eKey::KP8, "KP8" },
        { eKey::KP9, "KP9" },
        { eKey::KPDecimal, "KPDecimal" },
        { eKey::KPDivide, "KPDivide" },
        { eKey::KPMultiply, "KPMultiply" },
        { eKey::KPSubtract, "KPSubtract" },
        { eKey::KPAdd, "KPAdd" },
        { eKey::KPEnter, "KPEnter" },
        { eKey::KPEqual, "KPEqual" },

        { eKey::LeftShift, "LeftShift" },
        { eKey::LeftControl, "LeftControl" },
        { eKey::LeftAlt, "LeftAlt" },
        { eKey::LeftSuper, "LeftSuper" },
        { eKey::RightShift, "RightShift" },
        { eKey::RightControl, "RightControl" },
        { eKey::RightAlt, "RightAlt" },
        { eKey::RightSuper, "RightSuper" },

        { eKey::Menu, "Menu" },
    })

} // namespace xpe::core

// XML for MouseCodes
namespace xpe::core
{

    XmlEnum(eMouse, {

        { eMouse::NoneButton, "NoneButton" },

        { eMouse::Button0, "Button0" },
        { eMouse::Button1, "Button1" },
        { eMouse::Button2, "Button2" },
        { eMouse::Button3, "Button3" },
        { eMouse::Button4, "Button4" },
        { eMouse::Button5, "Button5" },
        { eMouse::Button6, "Button6" },
        { eMouse::Button7, "Button7" },

        { eMouse::ButtonLast, "ButtonLast" },
        { eMouse::ButtonLeft, "ButtonLeft" },
        { eMouse::ButtonRight, "ButtonRight" },
        { eMouse::ButtonMiddle, "ButtonMiddle" },

    })

} // namespace xpe::core

// XML for GamepadCodes
namespace xpe::core
{

    XmlEnum(eGamepadButton, {

        { eGamepadButton::Pad_A, "Pad_A" },
        { eGamepadButton::Pad_B, "Pad_B" },
        { eGamepadButton::Pad_X, "Pad_X" },
        { eGamepadButton::Pad_Y, "Pad_Y" },

        { eGamepadButton::Pad_LeftBumper, "Pad_LeftBumper" },
        { eGamepadButton::Pad_RightBumper, "Pad_RightBumper" },

        { eGamepadButton::Pad_Back, "Pad_Back" },
        { eGamepadButton::Pad_Start, "Pad_Start" },
        { eGamepadButton::Pad_Guide, "Pad_Guide" },

        { eGamepadButton::Pad_LeftThumb, "Pad_LeftThumb" },
        { eGamepadButton::Pad_RightThumb, "Pad_RightThumb" },

        { eGamepadButton::Pad_Up, "Pad_Up" },
        { eGamepadButton::Pad_Right, "Pad_Right" },
        { eGamepadButton::Pad_Down, "Pad_Down" },
        { eGamepadButton::Pad_Left, "Pad_Left" },

        { eGamepadButton::Pad_Last, "Pad_Last" },

        { eGamepadButton::Pad_Cross, "Pad_Cross" },
        { eGamepadButton::Pad_Circle, "Pad_Circle" },
        { eGamepadButton::Pad_Square, "Pad_Square" },
        { eGamepadButton::Pad_Triangle, "Pad_Triangle" },
    })

    XmlEnum(eGamepadAxis, {

        { eGamepadAxis::Axis_LeftX, "Axis_LeftX" },
        { eGamepadAxis::Axis_LeftY, "Axis_LeftY" },
        { eGamepadAxis::Axis_RightX, "Axis_RightX" },
        { eGamepadAxis::Axis_RightY, "Axis_RightY" },

        { eGamepadAxis::Axis_LeftTrigger, "Axis_LeftTrigger" },
        { eGamepadAxis::Axis_RightTrigger, "Axis_RightTrigger" },

        { eGamepadAxis::Axis_Last, "Axis_Last" }

    })

} // namespace xpe::core

// XML for Spaces
namespace xpe::core {

    XmlEnum(eSpace, {
        { eSpace::SPACE_2D, "2D" },
        { eSpace::SPACE_3D, "3D" }
    })

} // namespace xpe::core