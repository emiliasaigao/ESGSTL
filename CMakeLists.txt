cmake_minimum_required(VERSION 3.12)

project(ESGSTL)

# 设置源文件
set(SOURCES src/test.cpp)

include_directories(include)

file(GLOB SRC_FILES "include/*.h")

add_executable(esgstl ${SRC_FILES} ${SOURCES})