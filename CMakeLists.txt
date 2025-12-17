cmake_minimum_required(VERSION 3.28)

project(tpool C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# ThreadPool library
add_library(tpool STATIC src/tpool.c)


# Making it available for any include paths
target_include_directories(tpool PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>)

# Add pthread if needed
target_link_libraries(tpool PRIVATE pthread)

# add_executable(tpool "src/tpool.c")


#Testing
option(BUILD_TESTING "Build the testing tree" ON)
if(BUILD_TESTING)
    enable_testing()

    add_executable(tpool_test test/apitest_create_destroy.c
    test/apitest_addingtasks.c
    )

    target_link_libraries(tpool_test tpool)
    add_test(NAME tpool_test COMMAND tpool_test)
endif()

# Add includes
# target_include_directories(${PROJECT_NAME} PRIVATE tpool)
