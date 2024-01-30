#pragma once

namespace xpe::core {}
namespace xpe::ecs {}
namespace xpe::audio {}
namespace xpe::render {}
namespace xpe::anim {}
namespace xpe::math {}
namespace xpe::physics {}
namespace xpe::res {}
namespace xpe::ui {}

using namespace xpe::core;
using namespace xpe::ecs;
using namespace xpe::audio;
using namespace xpe::render;
using namespace xpe::anim;
using namespace xpe::math;
using namespace xpe::physics;
using namespace xpe::res;
using namespace xpe::ui;

// skip non-critical Win API warnings
#pragma warning(disable : 4081)
#pragma warning(disable : 4103)
#pragma warning(disable : 4067)
#pragma warning(disable : 4251) // dll-interface warning

// Macros
#include <macro_platform.hpp>
#include <macro_build.hpp>
#include <macro_constants.hpp>
#include <macro_debugbreak.hpp>
#include <macro_icons.hpp>

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
#include <types.hpp>
#include <profile_manager.hpp>
#include <memory_manager.hpp>
#include <allocators.hpp>
#include <smart_pointers.hpp>
#include <codes_keyboard.hpp>
#include <codes_mouse.hpp>
#include <codes_gamepad.hpp>
#include <json.hpp>
#include <xml.hpp>
#include <app_config.hpp>
#include <hardware_manager.hpp>

// STL customization
#include <stl.hpp>
#include <ring_buffer.hpp>

// Core
#include <log_manager.hpp>
#include <thread.hpp>
#include <task_manager.hpp>
#include <event_manager.hpp>
#include <file_manager.hpp>
#include <window_manager.hpp>
#include <input_manager.hpp>
#include <proc_manager.hpp>
#include <assimp_manager.hpp>