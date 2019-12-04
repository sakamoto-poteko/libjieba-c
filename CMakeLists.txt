cmake_minimum_required(VERSION 2.8)

project(jieba-c)

#add_subdirectory(cppjieba)

include_directories("include")
include_directories("cppjieba/include")
include_directories("cppjieba/deps")

add_definitions(-DLIBBUILD)

set(LIBJIEBA_HDR
    include/libjieba.h
    )
set(LIBJIEBA_SRC
    src/libjieba.cpp
    )

set(DEPENDS
    )

add_library(${PROJECT_NAME} SHARED ${LIBJIEBA_HDR} ${LIBJIEBA_SRC})
#target_link_libraries(${PROJECT_NAME} ${DEPENDS})
