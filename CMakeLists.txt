cmake_minimum_required(VERSION 3.18)
project(rl_render C)
set(CMAKE_C_STANDARD 11)

if(WIN32)
    set(OPENGL_LIBS opengl32 gdi32)
else()
    find_package(OpenGL REQUIRED)
    set(OPENGL_LIBS OpenGL::GL)
endif()

# directories
set(LIB_DIR "${CMAKE_SOURCE_DIR}/lib")
set(OUT_DIR "${CMAKE_SOURCE_DIR}/bin")
set(OBJ_DIR "${OUT_DIR}/obj")
set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")

# glfw
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)
set(GLFW_INSTALL OFF)
add_subdirectory(${LIB_DIR}/glfw)

# source
file(GLOB_RECURSE RL_RENDER_SRC "${SRC_DIR}/*.c")
set(SRC ${RL_RENDER_SRC})

# binaries
set(EXECUTABLE "rlr_exec")
set(LIBRARY "rlr")
add_library(${LIBRARY} ${SRC})
add_executable(${EXECUTABLE} main.c)

# incudes
target_include_directories(${LIBRARY} PRIVATE ${LIB_DIR} ${SRC_DIR} ${LIB_DIR}/glfw/include)
target_include_directories(${EXECUTABLE} PRIVATE ${LIB_DIR} ${SRC_DIR} ${LIB_DIR}/glfw/include)

# library linking
target_link_libraries(${LIBRARY} glfw ${OPENGL_LIBS})
target_link_libraries(${EXECUTABLE} ${LIBRARY})

target_compile_options(${LIBRARY} PRIVATE -O3 -pg)
target_compile_options(${EXECUTABLE} PRIVATE -O3 -pg)

target_link_options(${LIBRARY} PRIVATE -pg)
target_link_options(${EXECUTABLE} PRIVATE -pg)