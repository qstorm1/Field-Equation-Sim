cmake_minimum_required(VERSION 3.31)
project(Gravitational_Lensing_Simulation)

set(CMAKE_CXX_STANDARD 20)

add_executable(Gravitational_Lensing_Simulation
        "src/main.cpp"
        src/testFile.cpp
        src/testFile.h
        src/config.h
        src/Object/object.cpp
        src/Object/object.h
        src/Logging/logger.h
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
