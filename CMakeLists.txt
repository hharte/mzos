# CMakeList.txt : CMake project for mzos, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("mzos")

# Add source to this project's executable.
add_executable (mzos "mzos.c" )

set(CMAKE_FIND_LIBRARY_SUFFIXES ".lib")
if (MSVC)
    target_compile_options(mzos PRIVATE /MT)
    target_link_options(mzos PRIVATE /INCREMENTAL:NO /NODEFAULTLIB:MSVCRT)
endif (MSVC)

# Install
if (NOT MSVC)
install(TARGETS mzos DESTINATION /usr/local/bin)
endif()
