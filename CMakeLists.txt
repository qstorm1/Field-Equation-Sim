cmake_minimum_required(VERSION 3.31)
project(Gravitational_Lensing_Simulation)

include(FetchContent)

FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG
)

FetchContent_MakeAvailable(glfw)

add_executable(Gravitational_Lensing_Simulation src/main.cpp)

target_link_libraries(Gravitational_Lensing_Simulation PUBLIC glfw)

set(CMAKE_CXX_STANDARD 20)

