cmake_minimum_required(VERSION 3.13)
project(Artemis)

set(CMAKE_CXX_STANDARD 14)
include_directories(imgui/include)
file(GLOB IMGUI_FILES imgui/*.cpp)

add_executable(Artemis main.cpp ${IMGUI_FILES} danmuku.cpp texture.cpp image.cpp image.h)
target_link_libraries(Artemis d3d11)
