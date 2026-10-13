cmake_minimum_required(VERSION 3.16)
project(MiniOS VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_Declare(glfw GIT_REPOSITORY https://github.com/glfw/glfw.git GIT_TAG 3.4)
FetchContent_Declare(imgui GIT_REPOSITORY https://github.com/ocornut/imgui.git GIT_TAG v1.91.8)
FetchContent_MakeAvailable(glfw imgui)
find_package(OpenGL REQUIRED)

add_library(minios_core
  src/core/Scheduler.cpp src/core/MemoryManager.cpp
  src/core/PageReplacement.cpp src/core/Deadlock.cpp)
target_include_directories(minios_core PUBLIC src)

add_library(imgui_lib
  ${imgui_SOURCE_DIR}/imgui.cpp ${imgui_SOURCE_DIR}/imgui_draw.cpp
  ${imgui_SOURCE_DIR}/imgui_tables.cpp ${imgui_SOURCE_DIR}/imgui_widgets.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp)
target_include_directories(imgui_lib PUBLIC ${imgui_SOURCE_DIR} ${imgui_SOURCE_DIR}/backends)
target_link_libraries(imgui_lib PUBLIC glfw OpenGL::GL)

add_executable(MiniOS
  src/main.cpp src/gui/App.cpp src/gui/Widgets.cpp
  src/gui/SchedulerPanel.cpp src/gui/MemoryPanel.cpp
  src/gui/PagingPanel.cpp src/gui/DeadlockPanel.cpp)
target_link_libraries(MiniOS PRIVATE minios_core imgui_lib)
if(MSVC)
  target_compile_options(MiniOS PRIVATE /W4 /permissive-)
else()
  target_compile_options(MiniOS PRIVATE -Wall -Wextra -Wpedantic)
endif()

enable_testing()
add_executable(core_tests tests/core_tests.cpp)
target_link_libraries(core_tests PRIVATE minios_core)
add_test(NAME core_tests COMMAND core_tests)
