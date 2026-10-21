cmake_minimum_required(VERSION 3.31)
project(Gravitational_Lensing_Simulation)

include(FetchContent)

FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG
)
FetchContent_MakeAvailable(glfw)

FetchContent_Declare(
        glad
        GIT_REPOSITORY https://github.com/Dav1dde/glad.git
        GIT_TAG
)


FetchContent_MakeAvailable(glad)

add_executable(Gravitational_Lensing_Simulation
        "src/main.cpp"
        src/testFile.cpp
        src/testFile.h
        src/config.h
        src/objects/object.cpp
        src/objects/object.h
        src/logging/logger.h
)
target_link_libraries(Gravitational_Lensing_Simulation PUBLIC glfw)
target_link_libraries(Gravitational_Lensing_Simulation PUBLIC glad)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
