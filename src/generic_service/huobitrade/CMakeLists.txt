# CMakeList.txt : CMake project for huobitrade, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project(huobitrade)

# include路径
include_directories(
${CMAKE_SOURCE_DIR}
${CMAKE_SOURCE_DIR}/huobitrade
${CMAKE_SOURCE_DIR}/zlib-1.2.11
${CMAKE_BINARY_DIR}/zlib-1.2.11
${CMAKE_SOURCE_DIR}/cares-1.16.0
${CMAKE_BINARY_DIR}/libiconv-1.16
${CMAKE_SOURCE_DIR}/libssh2-1.9.0/include
${CMAKE_BINARY_DIR}/libssh2-1.9.0/src
${CMAKE_SOURCE_DIR}/libiconv-1.16/include  
${CMAKE_BINARY_DIR}/curl-7.69.1
${CMAKE_SOURCE_DIR}/curl-7.69.1/include 
${CMAKE_SOURCE_DIR}/libuv-1.36.0/include 
${CMAKE_SOURCE_DIR}/openssl-1.1.1f/include
${CMAKE_BINARY_DIR}/openssl-1.1.1f/include
${CMAKE_SOURCE_DIR}/rapidjson-1.1.0/include
${CMAKE_SOURCE_DIR}/uwebsockets/src 
${CMAKE_SOURCE_DIR}/uwebsockets/src/include
${CMAKE_SOURCE_DIR}/uwebsockets/usockets/src
)

# 把第三方库的.a文件和.so文件放到代码的lib文件夹
link_directories(${CMAKE_BINARY_DIR}/uwebsockets)
link_directories(${CMAKE_BINARY_DIR}/zlib-1.2.11)
link_directories(${CMAKE_BINARY_DIR}/libuv-1.36.0)
link_directories(${CMAKE_BINARY_DIR}/libiconv-1.16)
link_directories(${CMAKE_BINARY_DIR}/libssh2-1.9.0/src)
link_directories(${CMAKE_BINARY_DIR}/openssl-1.1.1f/ssl)
link_directories(${CMAKE_BINARY_DIR}/openssl-1.1.1f/crypto)
link_directories(${CMAKE_BINARY_DIR}/cares-1.16.0/${CMAKE_INSTALL_LIBDIR})
link_directories(${CMAKE_BINARY_DIR}/libcurl-7.69.1/${CMAKE_INSTALL_LIBDIR})

set(CMAKE_C_FLAGS "$ENV{CFLAGS} -DLIBUS_USE_LIBUV -DLIBUS_USE_OPENSSL -std=c11 -flto")

list(APPEND EXTERN_LIBRARIES 
    ssl_static
    crypto_static
    cares_static
    uv_static
    zlib_static
    ssh2_static
    iconv_static
    libcurl_static
    usockets_static)

if (MSVC)
	set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} ${CMAKE_CXX_FLAGS} /std:c++17 /std:c++latest -D_HAS_CXX17=1 -D_HAS_CXX20=1")
	#set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} ${CMAKE_CXX_FLAGS} -DUWS_NO_ZLIB /std:c++17 /std:c++latest -D_HAS_CXX17=1 -D_HAS_CXX20=1")
else()
	set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} ${CMAKE_CXX_FLAGS} -std=c++17")
    list(APPEND EXTERN_LIBRARIES pthread stdc++)
endif(MSVC)

# Add source to this project's executable.
add_executable (huobitrade "main.cpp")
#add_library (huobi-signature STATIC "signature.cpp" "base64.h")

target_link_libraries(huobitrade ${EXTERN_LIBRARIES})
# TODO: Add tests and install targets if needed.
