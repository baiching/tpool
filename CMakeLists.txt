cmake_minimum_required(VERSION 3.28)

project(tpool C)
# message("CMake found C compiler: ${CMAKE_C_COMPILER}")
set(CMAKE_C_COMPILER "C:/MinGW/bin/gcc.exe")
#
# if(NOT EXISTS "${CMAKE_C_COMPILER}")
#     message(FATAL_ERROR "C compiler not found: ${CMAKE_C_COMPILER}")
# endif()

# set(CMAKE_C_STANDARD 11)
# set(CMAKE_C_STANDARD_REQUIRED ON)

# Add all .c files in src/ automatically
# file(GLOB SOURCES "src/*.c")

add_executable(tpool "src/tpool.c")

# Add includes
target_include_directories(${PROJECT_NAME} PRIVATE tpool)

# Add pthread if needed
target_link_libraries(tpool PRIVATE pthread)
