cmake_minimum_required(VERSION 3.28)

project(tpool C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# ThreadPool library
add_library(tpool STATIC src/tpool.c
                         src/tpool.h)


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

    # test1
    add_executable(tpool_test1 test/apitest_create_destroy.c)
    target_link_libraries(tpool_test1 tpool)
    add_dependencies(tpool_test1 tpool)
    add_test(NAME tpool_test1 COMMAND tpool_test1)

    #test2
    add_executable(tpool_test2 test/apitest_addingtasks.c)
    target_link_libraries(tpool_test2 tpool)
    add_dependencies(tpool_test2 tpool)
    add_test(NAME tpool_test2 COMMAND tpool_test2)

    #test3
    add_executable(tpool_test3 test/apitest_completion.c)
    target_link_libraries(tpool_test3 tpool)
    add_dependencies(tpool_test3 tpool)
    add_test(NAME tpool_test3 COMMAND tpool_test3)

endif()

# Add includes
# target_include_directories(${PROJECT_NAME} PRIVATE tpool)

# set_source_files_properties(src/tpool.c PROPERTIES OBJECT_DEPENDS src/tpool.h)
