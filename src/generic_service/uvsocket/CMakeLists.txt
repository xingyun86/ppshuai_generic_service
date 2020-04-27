# CMakeList.txt : CMake project for uvsocket, include source and define
# project specific logic here.
#
# CMake 最低版本号要求
cmake_minimum_required (VERSION 3.8)

# 项目信息
project (uvsocket)

MESSAGE(STATUS "CMAKE_BINARY_DIR=" ${CMAKE_BINARY_DIR})
MESSAGE(STATUS "CMAKE_SOURCE_DIR=" ${CMAKE_SOURCE_DIR})
MESSAGE(STATUS "PROJECT_BINARY_DIR=" ${PROJECT_BINARY_DIR})
MESSAGE(STATUS "PROJECT_SOURCE_DIR=" ${PROJECT_SOURCE_DIR})
MESSAGE(STATUS "uvsocket_BINARY_DIR=" ${uvsocket_BINARY_DIR})
MESSAGE(STATUS "uvsocket_SOURCE_DIR=" ${uvsocket_SOURCE_DIR})

# include路径
include_directories(${CMAKE_SOURCE_DIR} 
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
${CMAKE_SOURCE_DIR}/uws/helper
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

# 设置可执行文件生成路径
#set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

# 生成debug版本
#set(CMAKE_BUILD_TYPE "Release") 
#set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
#set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

# 查找指定源文件
set(SRCS "uvsocket.cpp")

# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_LIB_SRCS 变量
aux_source_directory(. DIR_SRCS)

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

if (MSVC)
    set(CompilerFlags
        CMAKE_CXX_FLAGS
        CMAKE_CXX_FLAGS_DEBUG
        CMAKE_CXX_FLAGS_RELEASE
        CMAKE_C_FLAGS
        CMAKE_C_FLAGS_DEBUG
        CMAKE_C_FLAGS_RELEASE
        )
    foreach(CompilerFlag ${CompilerFlags})
        string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
    endforeach()
else()
    set(CMAKE_C_FLAGS "$ENV{CFLAGS} ${CMAKE_C_FLAGS} -fPIC")
    set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} ${CMAKE_CXX_FLAGS} -fPIC")
endif(MSVC)

# 指定生成目标 
# Add source to this project's executable.
add_executable (uvsocket ${SRCS} ${DIR_SRCS})

# 链接第三方静态库
target_link_libraries(uvsocket ${EXTERN_LIBRARIES})
# 链接第三方动态库 
#target_link_libraries(uvsocket -luv)

# TODO: Add tests and install targets if needed.
