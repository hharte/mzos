# CMakeList.txt : CMake project for mzos
#
cmake_minimum_required (VERSION 3.8)

project ("mzos")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add source to this project's executable.
add_executable (mzos "mzos.c" "mzos.h")

set(CMAKE_FIND_LIBRARY_SUFFIXES ".lib")
if (MSVC)
    target_compile_options(mzos PRIVATE /MT)
    target_link_options(mzos PRIVATE /INCREMENTAL:NO /NODEFAULTLIB:MSVCRT)
endif (MSVC)

# Install
if (NOT MSVC)
install(TARGETS mzos DESTINATION /usr/local/bin)
endif()
