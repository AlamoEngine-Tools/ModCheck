cmake_minimum_required(VERSION 3.18)
project(ModCheck)

add_subdirectory(lua-5.0.3)

file(GLOB SOURCE_FILES *.cpp Assets/*.cpp General/*.cpp)
file(GLOB HEADER_FILES *.h Assets/*h General/*h)

add_library(ModCheck ${SOURCE_FILES} ${HEADER_FILES})
target_include_directories(ModCheck PUBLIC ${CMAKE_CURRENT_LIST_DIR})
