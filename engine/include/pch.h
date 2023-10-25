#pragma once

// skip non-critical Win API warnings
#pragma warning(disable : 4081)
#pragma warning(disable : 4103)
#pragma warning(disable : 4067)
#pragma warning(disable : 4251) // dll-interface warning

// Platform Detection macros
#include <core/macro_platform.h>

// STL
#define _ENFORCE_MATCHING_ALLOCATORS 0
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
#include <regex>
#include <atomic>
#include <condition_variable>

// GLM
#include <glm.hpp>
#include <gtx/quaternion.hpp>
#include <gtc/type_ptr.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtx/matrix_transform_2d.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/string_cast.hpp>

// Core
#include <core/macro_build.h>
#include <core/types.hpp>
#include <core/memory_manager.hpp>
#include <core/allocators.h>
#include <core/smart_pointers.h>
#include <core/codes_keyboard.h>
#include <core/codes_mouse.h>
#include <core/codes_gamepad.h>
#include <res/json.h>
#include <res/xml.h>
#include <core/app_config.h>
#include <core/hardware_manager.h>

// Rendering constants
#include <rendering/core/slots.h>

// STL customization
#include <stl/string.h>
#include <stl/array.h>
#include <stl/vector.h>
#include <stl/unordered_map.h>
#include <stl/map.h>
#include <stl/sstream.h>
#include <stl/ring_buffer.h>

// Core
#include <core/time_manager.hpp>
#include <core/log_manager.h>
#include <core/macro_debugbreak.h>
#include <core/thread.h>
#include <core/task_manager.h>
#include <core/file_manager.h>
#include <core/window_manager.hpp>
#include <core/input_manager.h>

// Math
#include <math/math.h>
#include <math/transform.h>
#include <math/matrices.h>
#include <math/raycast.h>

// ECS
#include <ecs/ecs.h>