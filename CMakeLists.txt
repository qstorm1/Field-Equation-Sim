cmake_minimum_required(VERSION 3.15.0)
project(Gravitational_Lensing_Simulation)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

find_package(OpenGL REQUIRED)

if(UNIX AND NOT APPLE)

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
    FetchContent_Declare(
            glm
            GIT_REPOSITORY https://github.com/g-truc/glm.git
            GIT_TAG
    )

    FetchContent_MakeAvailable(glm)


    FetchContent_MakeAvailable(glad)

    add_executable(Gravitational_Lensing_Simulation
            "src/main.cpp"
            src/config.h
            src/Object/object3d.h
            src/Logging/logger.h
            src/Graphics/graphics.h
            src/GlobalVariables.h
            src/matrix.hpp
            src/matrix.h
    )
    target_link_libraries(Gravitational_Lensing_Simulation PUBLIC
            glfw
            glad
            OpenGL::GL
            glm
    )

    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)

elseif(WIN32 OR WIN64)

    #idk how to import glm using files

    set(CMAKE_CXX_STANDARD 20)

    add_executable(Gravitational_Lensing_Simulation
            "src/main.cpp"
            src/config.h
            src/Object/object3d.h
            src/Logging/logger.h
            src/Graphics/graphics.h
            src/GlobalVariables.h
            dependencies/windows/glad/glad.c
            #https://github.com/XingGao-cn/FreeMatrix-C
            src/matrix.hpp
            src/matrix.h
    )
    target_include_directories(Gravitational_Lensing_Simulation PRIVATE dependencies/windows)

    target_link_libraries(Gravitational_Lensing_Simulation
            ${CMAKE_SOURCE_DIR}/windows/dependencies/GLFW/libglfw3.a
            opengl32)
    #gdi32 doesn't work on linux so we shouldn't import it, you can still install it if it says it needs it but don't include it in any header files
endif ()
