#cmake_minimum_required(VERSION 3.15.0)
#project(Gravitational_Lensing_Simulation)
#
#set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
#
#include(FetchContent)
#
#FetchContent_Declare(
#        glfw
#        GIT_REPOSITORY https://github.com/glfw/glfw.git
#        GIT_TAG
#)
#FetchContent_MakeAvailable(glfw)
#
#FetchContent_Declare(
#        glad
#        GIT_REPOSITORY https://github.com/Dav1dde/glad.git
#        GIT_TAG
#)
#
#
#FetchContent_MakeAvailable(glad)
#
#add_executable(Gravitational_Lensing_Simulation
#        "src/main.cpp"
#        src/testFile.cpp
#        src/testFile.h
#        src/config.h
#        src/Object/object.cpp
#        src/Object/object.h
#        src/Logging/logger.h
#)
#target_link_libraries(Gravitational_Lensing_Simulation PUBLIC glfw)
#target_link_libraries(Gravitational_Lensing_Simulation PUBLIC glad)
#
#set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)




#uhhhh yeah I have to do this to load dependencies idk why. try this yourself too if it works for both then let's keep this version

cmake_minimum_required(VERSION 3.15.0)
project(Gravitational_Lensing_Simulation)

set(CMAKE_CXX_STANDARD 20)

add_executable(Gravitational_Lensing_Simulation
                "src/main.cpp"
                src/config.h
                src/Object/object.cpp
                src/Object/object.h
                src/Logging/logger.h
)

target_include_directories(Gravitational_Lensing_Simulation PRIVATE dependencies)

target_link_libraries(Gravitational_Lensing_Simulation
        ${CMAKE_SOURCE_DIR}/dependencies/GLFW/libglfw3.a
        opengl32
        gdi32)