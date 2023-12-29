#pragma once

// skip non-critical Win API warnings
#pragma warning(disable : 4081)
#pragma warning(disable : 4103)
#pragma warning(disable : 4067)
#pragma warning(disable : 4251) // dll-interface warning

// Macros
#include <core/macro_platform.hpp>
#include <core/macro_build.hpp>
#include <core/macro_constants.hpp>
#include <core/macro_debugbreak.hpp>

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
#include <core/types.hpp>
#include <core/memory_manager.hpp>
#include <core/allocators.hpp>
#include <core/smart_pointers.hpp>
#include <core/codes_keyboard.hpp>
#include <core/codes_mouse.hpp>
#include <core/codes_gamepad.hpp>
#include <res/json.hpp>
#include <res/xml.hpp>
#include <core/app_config.hpp>
#include <core/hardware_manager.hpp>

// STL customization
#include <stl/stl.hpp>
#include <stl/ring_buffer.hpp>

// Core
#include <core/time_manager.hpp>
#include <core/log_manager.hpp>
#include <core/thread.hpp>
#include <core/task_manager.hpp>
#include <core/event_manager.hpp>
#include <core/observers.hpp>
#include <core/file_manager.hpp>
#include <core/window_manager.hpp>
#include <core/input_manager.hpp>

// Physics
#include <physics/physics_manager.hpp>

// Math
#include <math/math_manager.hpp>

// ECS
#include <ecs/ecs.hpp>

using namespace xpe::core;
using namespace xpe::ecs;