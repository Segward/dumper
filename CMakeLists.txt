cmake_minimum_required(VERSION 3.25)
project(dumper LANGUAGES CXX OBJCXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-Wall -Wextra)

add_executable(dumper src/dumper/main.cpp)

add_library(dumper_dylib SHARED
  src/dumper/util.cpp
  src/dumper/segments.cpp
  src/dumper/scheduler.cpp
  src/dumper/dumper.cpp
)

target_include_directories(dumper_dylib PUBLIC include)
target_precompile_headers(dumper_dylib PRIVATE include/dumper/common.hpp)
set_target_properties(dumper_dylib PROPERTIES OUTPUT_NAME "dumper")
