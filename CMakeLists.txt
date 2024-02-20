cmake_minimum_required(VERSION 3.12)

project(ESGSTL)

# 设置源文件
set(SOURCES src/test.cpp)

# 将 include 目录添加到头文件搜索路径
include_directories(include)

# 声明可执行文件目标
add_executable(my_executable ${SOURCES})
