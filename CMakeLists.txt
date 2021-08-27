# CMakeList.txt : CMake project for idTranslate, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("idTranslate")

if(MSVC)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
endif(MSVC)

# Add source to this project's executable.
add_executable (idTranslate "tserver.cpp")

# TODO: Add tests and install targets if needed.
