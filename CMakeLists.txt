cmake_minimum_required(VERSION 3.1)
project(HnSRTree)

set(CMAKE_CXX_STANDARD 20)

find_package(Threads REQUIRED)
include_directories(include)
include_directories(obj_inc)
include_directories(rect_inc)
include_directories(sr_tree_inc)


file(GLOB sr_tree_src
        obj_src/*.cc rect_src/*.cc sr_tree_src/*.cc src/*.cc)
add_library(sr_tree_lib ${sr_tree_src})

SET(EXAMPLES
        hello
        rs
        checkSRTree
        createSRTree
        printSRTree
        removeSRTree
        searchSRTree
        storeSRTree
        )
FOREACH(EXAMPLE ${EXAMPLES} )
add_executable(${EXAMPLE} app/${EXAMPLE}.cc)
target_link_libraries(${EXAMPLE} PRIVATE sr_tree_lib)
ENDFOREACH(EXAMPLE)

