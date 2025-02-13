cmake_minimum_required(VERSION 3.13)

# Set the C and C++ standards
set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 20)

project(picoutil C CXX ASM)

add_executable(${PROJECT_NAME}
    src/picoutil.c
)
