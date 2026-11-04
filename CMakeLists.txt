cmake_minimum_required(VERSION 3.15.0)
project(Gravitational_Lensing_Simulation)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)


include(FetchContent)

# ---- SymEngine (symbolic math) ----
#I HATE cmake
FetchContent_Declare(
        boost_headers
        URL https://github.com/boostorg/boost/releases/download/boost-1.86.0/boost-1.86.0-b2-nodocs.tar.xz
        SOURCE_SUBDIR do_not_build_boost
)
FetchContent_MakeAvailable(boost_headers)
set(Boost_INCLUDE_DIR "${boost_headers_SOURCE_DIR}" CACHE PATH "" FORCE)
set(BOOST_ROOT "${boost_headers_SOURCE_DIR}" CACHE PATH "" FORCE)

# SymEngine itself
set(INTEGER_CLASS "boostmp" CACHE STRING "" FORCE)
set(WITH_SYMENGINE_RCP ON CACHE BOOL "" FORCE)
set(BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(BUILD_BENCHMARKS OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
        symengine
        GIT_REPOSITORY https://github.com/symengine/symengine.git
        GIT_TAG v0.14.0
        GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(symengine)

if(UNIX AND NOT APPLE)

    include(FetchContent)

    FetchContent_Declare(
            glfw
            GIT_REPOSITORY https://github.com/glfw/glfw.git
            GIT_TAG 3.4
    )

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

    FetchContent_MakeAvailable(glfw glad glm)

    add_executable(Gravitational_Lensing_Simulation
            src/main.cpp
            src/Shader/shader.h
            src/Shader/shader.cpp
    )
    target_link_libraries(Gravitational_Lensing_Simulation PUBLIC
            glfw
            glad
            OpenGL::GL
            glm::glm
            #DOUBLE CHECK THIS WORKS PROPERLY ON LINUX
            symengine
    )

    #got this from AI (CMake has trash documentation :/)

    install(TARGETS Gravitational_Lensing_Simulation
            RUNTIME DESTINATION bin
            COMPONENT app)

    set(CPACK_DEB_COMPONENT_INSTALL ON)
    set(CPACK_COMPONENTS_ALL app)
    set(CPACK_DEBIAN_APP_FILE_NAME DEB-DEFAULT)
    set(CPACK_DEBIAN_APP_PACKAGE_NAME "gravitational-lensing-simulation")
    set(CPACK_GENERATOR "DEB")
    set(CPACK_PACKAGE_NAME "gravitational-lensing-simulation")
    set(CPACK_PACKAGE_VERSION "0.1.1")
    set(CPACK_PACKAGE_CONTACT "Qstorm don't email")
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Gravitational lensing simulation")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Qstorm don't email")
    set(CPACK_DEBIAN_PACKAGE_SECTION "science")
    set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
    set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)  # auto-detect runtime deps (needs dpkg-shlibdeps)
    include(CPack)



    

elseif(WIN32 OR WIN64)

    #idk how to import glm using files

    set(CMAKE_CXX_STANDARD 20)

    add_executable(Gravitational_Lensing_Simulation
            "src/main.cpp"
            src/Shader/shader.cpp
            src/Shader/shader.h
    )
    target_include_directories(Gravitational_Lensing_Simulation PRIVATE dependencies/windows)

    target_link_libraries(Gravitational_Lensing_Simulation
            ${CMAKE_SOURCE_DIR}/dependencies/windows/GLFW/libglfw3.a
            opengl32
            symengine)
    #gdi32 doesn't work on linux so we shouldn't import it, you can still install it if it says it needs it but don't include it in any header files
endif ()

target_include_directories(Gravitational_Lensing_Simulation PRIVATE
        ${symengine_SOURCE_DIR}
        ${symengine_BINARY_DIR}
        ${boost_headers_SOURCE_DIR})

#The location of the shader folder
target_compile_definitions(Gravitational_Lensing_Simulation PRIVATE SHADER_DIR="${CMAKE_SOURCE_DIR}/src/Shader/shaders/")
