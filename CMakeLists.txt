cmake_minimum_required(VERSION 3.5)
project(mnn-tts)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/source-charset:utf-8>")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
endif()

set(MNN_LOW_MEMORY ON CACHE BOOL "Open MNN_LOW_MEMORY" FORCE)
set(MNN_SUPPORT_TRANSFORMER_FUSE ON CACHE BOOL "Open MNN_SUPPORT_TRANSFORMER_FUSE" FORCE)
set(MNN_BUILD_AUDIO ON CACHE BOOL "Open MNN_BUILD_AUDIO" FORCE)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/MNN)

# include dir
include_directories(${CMAKE_CURRENT_LIST_DIR}/include/
        ${CMAKE_CURRENT_LIST_DIR}/src/3rd_include/
        ${CMAKE_CURRENT_LIST_DIR}/MNN/include/
        ${CMAKE_CURRENT_LIST_DIR}/MNN/3rd_party/
        ${CMAKE_CURRENT_LIST_DIR}/MNN/tools/audio/include/
)

# source files
FILE(GLOB SRCS ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/cpp-pinyin/*.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/zh_normalization/*.cpp)
add_executable(tts_demo ${SRCS})

target_link_libraries(tts_demo MNN MNN_Express MNNAudio)