#pragma once

#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>
#include <iterator>	

#include <string>
#include <sstream>
#include <fstream>
#include <array>
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>

#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers
#include <Windows.h>

#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>
#include <imgui_stdlib.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <ImGuizmo.h>

#include "Engine/Core/Constants.h"
#include "Engine/ECS/Base.h"
#include "Engine/Graphics/opengl.h"
#include "Engine/Utils/Log.h"
