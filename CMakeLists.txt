cmake_minimum_required(VERSION 3.16)

project(ComicConvert VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# RarLib
message(STATUS "Downloading RarLib...")

set(RARLIB_HEADER_ONLY OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
	RarLib
	GIT_REPOSITORY https://github.com/DostLeFan/RarLib.git
	GIT_TAG v0.2.0
	GIT_SHALLOW ON
)

FetchContent_MakeAvailable(RarLib)

message(STATUS "RarLib downloaded.")

# ZLIB
message(STATUS "Downloading zlib...")

set(ZLIB_USE_STATIC_LIBS ON)

FetchContent_Declare(
	zlib
	GIT_REPOSITORY https://github.com/madler/zlib.git
	GIT_TAG v1.3.2
	GIT_SHALLOW ON
)

FetchContent_MakeAvailable(zlib)

set(ZLIB_INCLUDE_DIR ${zlib_SOURCE_DIR} CACHE PATH "" FORCE)
set(ZLIB_LIBRARY zlib CACHE STRING "" FORCE)
set(ZLIB_FOUND TRUE CACHE BOOL "" FORCE)

message(STATUS "zlib downloaded.")

# libzip
message(STATUS "Downloading libzip...")

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(BUILD_TOOLS OFF CACHE BOOL "" FORCE)
set(BUILD_REGRESS OFF CACHE BOOL "" FORCE)
set(ENABLE_COMMONCRYPTO OFF CACHE BOOL "" FORCE)
set(ENABLE_GNUTLS OFF CACHE BOOL "" FORCE)
set(ENABLE_MBEDTLS OFF CACHE BOOL "" FORCE)
set(ENABLE_OPENSSL OFF CACHE BOOL "" FORCE)
set(ENABLE_BZIP2 OFF CACHE BOOL "" FORCE)
set(ENABLE_LZMA OFF CACHE BOOL "" FORCE)
set(ENABLE_ZSTD OFF CACHE BOOL "" FORCE)
set(LIBZIP_DO_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
	libzip
	GIT_REPOSITORY https://github.com/nih-at/libzip.git
	GIT_TAG v1.11.4
	GIT_SHALLOW ON
)

FetchContent_MakeAvailable(libzip)

message(STATUS "libzip downloaded.")

file(GLOB_RECURSE SOURCES RELATIVE ${CMAKE_SOURCE_DIR} "src/*.cpp")
file(GLOB_RECURSE HEADERS RELATIVE ${CMAKE_SOURCE_DIR} "include/*.hpp")

add_executable(${CMAKE_PROJECT_NAME} ${HEADERS} ${SOURCES})
target_compile_features(${CMAKE_PROJECT_NAME} PRIVATE cxx_std_17)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE RarLib::RarLib zip)

# On Windows : avoid console in release mode.
# if(WIN32)
	# set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
# endif()

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${CMAKE_PROJECT_NAME})