# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("generic_service")

set( CMAKE_VERBOSE_MAKEFILE on )

if(MSVC)
else()
set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++")
endif(MSVC)

# Include sub-projects.
add_subdirectory ("zlib-1.2.11")
# Include sub-projects.
add_subdirectory ("cares-1.16.0")
# Include sub-projects.
add_subdirectory ("libiconv-1.16")
# Include sub-projects.
#add_subdirectory ("openssl-1.0.2u")
# Include sub-projects.
add_subdirectory ("openssl-1.1.1f")
# Include sub-projects.
add_subdirectory ("libssh2-1.9.0")
# Include sub-projects.
add_subdirectory ("curl-7.69.1")
# Include sub-projects.
add_subdirectory ("libuv-1.36.0")
# Include sub-projects.
add_subdirectory ("uwebsockets")

# Include sub-projects.
add_subdirectory ("uws")
# Include sub-projects.
add_subdirectory ("uvsocket")
# Include sub-projects.
add_subdirectory ("huobitrade")
