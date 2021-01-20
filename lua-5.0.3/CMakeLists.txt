cmake_minimum_required(VERSION 3.18)
project(Lua5_0_3)

set(source_dir ${CMAKE_CURRENT_LIST_DIR}/src)
set(lib_dir ${source_dir}/lib)
set(lua_dir ${source_dir}/lua)
set(luac_dir ${source_dir}/luac)

file(GLOB LUA_SOURCE_FILES ${source_dir}/*.c ${lib_dir}/*.c ${lua_dir}/*.c ${luac_dir}/*.c)
file(GLOB LUA_HEADER_FILES ${source_dir}/*.h ${lib_dir}/*.h ${lua_dir}/*.h ${luac_dir}/*.h)

add_library(Lua ${LUA_SOURCE_FILES} ${LUA_HEADER_FILES})


