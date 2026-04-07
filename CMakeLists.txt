cmake_minimum_required(VERSION 3.20)
project(rl_render C)
set(CMAKE_C_STANDARD 11)

# platform
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(HOST_OS "linux")
    set(HOST_RPATH "-Wl,-rpath,'\$ORIGIN'")
    set(HOST_DYN ".so")
    set(HOST_EXE "")
    set(HOST_LIBS "-lX11 -lGL -lXrandr -lm")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(HOST_OS "macos")
    set(HOST_RPATH "-Wl,-rpath,@executable_path")
    set(HOST_DYN ".dylib")
    set(HOST_EXE "")
    set(HOST_LIBS "-framework Cocoa -framework CoreVideo -framework OpenGL -framework IOKit")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(HOST_OS "windows")
    set(HOST_RPATH "")
    set(HOST_DYN ".dll")
    set(HOST_EXE ".exe")
    set(HOST_LIBS "-lopengl32 -lgdi32")
else()
    message(FATAL_ERROR "Unsupported OS: ${CMAKE_SYSTEM_NAME}")
endif()

# arch
if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
    set(HOST_ARCH "x86_64")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "arm64|aarch64")
    set(HOST_ARCH "aarch64")
else()
    set(HOST_ARCH "unknown")
endif()

# directories
set(LIB_DIR "${CMAKE_SOURCE_DIR}/lib")
set(OUT_DIR "${CMAKE_SOURCE_DIR}/bin")
set(OBJ_DIR "${OUT_DIR}/obj")
set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")

# source
file(GLOB_RECURSE RL_RENDER_SRC "${SRC_DIR}/*.c")
set(SRC ${RL_RENDER_SRC} "main.c" "${LIB_DIR}/lib.c")

# executable
set(BINARY "rl_render${HOST_EXE}")
add_executable(${BINARY} ${SRC})

# incudes
target_include_directories(${BINARY} PRIVATE ${LIB_DIR})

# library linking
target_link_libraries(${BINARY} PRIVATE ${HOST_LIBS})

# compiler flags
if(MSVC)
    target_compile_options(${BINARY} PRIVATE /W4 /O3 /utf-8)
else()
    target_compile_options(${BINARY} PRIVATE -Wall -O3)
    target_link_options(${BINARY} PRIVATE ${HOST_RPATH})
endif()