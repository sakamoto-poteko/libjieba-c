cmake_minimum_required(VERSION 3.1)

project(jieba-c)

include_directories("include")
include_directories("cppjieba/include")
include_directories("cppjieba/deps")

# range-based loops, auto, etc...
set(CMAKE_CXX_STANDARD_REQUIRED 11)
set(CMAKE_CXX_STANDARD 11)


# u8 literal
set(CMAKE_C_STANDARD_REQUIRED 11)
set(CMAKE_C_STANDARD 11)

set(DEMO_SRC
	demo.c
	)

set(LIBJIEBA_HDR
    include/libjieba.h
    )
set(LIBJIEBA_SRC
    src/libjieba.cpp
    )

set(DEPENDS
    )

#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

add_library(${PROJECT_NAME} SHARED ${LIBJIEBA_HDR} ${LIBJIEBA_SRC})
target_compile_definitions(${PROJECT_NAME} PRIVATE LIBBUILD)
#target_link_libraries(${PROJECT_NAME} ${DEPENDS})

OPTION(BUILD_DEMO "Build the demo for this library" FALSE) 
IF(${BUILD_DEMO}) 
    add_executable(demo ${DEMO_SRC})
	target_link_libraries(demo ${PROJECT_NAME})
ENDIF()