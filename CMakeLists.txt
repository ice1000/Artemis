cmake_minimum_required(VERSION 3.11)
project(Artemis)

set(CMAKE_CXX_STANDARD 11)
include_directories(imgui/include)
file(GLOB IMGUI_FILES imgui/*.cpp)
set(LIBRARY_FILES danmuku.cpp texture.cpp image.cpp)

add_executable(Artemis main.cpp ${IMGUI_FILES} ${LIBRARY_FILES})
target_link_libraries(Artemis d3d11)
