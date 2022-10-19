project(Lab3)
cmake_minimum_required (VERSION 3.22)

set(CMAKE_CXX_STANDARD 14)
set(BUILD_SHARED_LIBS OFF)
set(CMAKE_EXE_LINKER_FLAGS "-static")

add_executable (Lab3 "main.cpp"  "tinyxml2.cpp" "tinyxml2.h")

# TODO: Добавьте тесты и целевые объекты, если это необходимо.
