#pragma once

// skip non-critical Win API warnings
#pragma warning(disable : 4081)
#pragma warning(disable : 4103)
#pragma warning(disable : 4067)
#pragma warning(disable : 4251) // dll-interface warning

// Platform Detection macros
#include <core/platform_detection.h>

// STL
#include <random>
#include <string>
#include <algorithm>
#include <functional>
#include <array>
#include <vector>
#include <thread>
#include <future>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <queue>
#include <memory>
#include <mutex>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <cassert>
#include <cstdarg>
#include <ctime>
#include <utility>
#include <chrono>
#include <iostream>

// GLM
#include <glm.hpp>
#include <gtx/quaternion.hpp>
#include <gtc/type_ptr.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtx/matrix_transform_2d.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/string_cast.hpp>

// Core
#include <core/build.h>
#include <core/types.hpp>
#include <core/singleton.h>
#include <core/config.h>
#include <core/memory_pool.hpp>
#include <rendering/shader_types.h>
#include <rendering/slots.h>

// STL customization
#include <stl/allocator.h>
#include <stl/string.h>
#include <stl/vector.h>
#include <stl/unordered_map.h>
#include <stl/map.h>

// Core
#include <core/ecs.hpp>
#include <core/time.h>
#include <core/timer.hpp>
#include <core/logger.h>
#include <core/debug_break.h>
#include <core/file_manager.h>

// Math
#include <math/math.h>
#include <math/matrices.h>
#include <math/sphere.h>
#include <math/raycast.h>