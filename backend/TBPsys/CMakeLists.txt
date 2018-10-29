#help: https://stackoverflow.com/questions/23626338/c-cmake-make-linker-ld-does-not-find-library-missing-path
cmake_minimum_required(VERSION 3.5.1)
project (Time_Terminal)
if(("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang") OR ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU") OR (("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel") AND UNIX))
  set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++14 ")
  set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
  set(CMAKE_CXX_FLAGS "-std=c++14 ")
endif()




set(SOURCES src/terminal-time.cpp src/base.cpp src/segment.cpp src/interpretation.cpp src/average.cpp src/transferfunction.cpp ./../OpenGL/src/glapplication.cpp ./../OpenGL/src/window.cpp)


################################################################################
# search
################################################################################
find_package(OpenCV REQUIRED)
find_package(OpenMP)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)

if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()


################################################################################
# include dir
################################################################################
find_package(OpenGL REQUIRED)
if (NOT OPENGL_FOUND)
    message (FATAL_ERROR "OpenGL not found!")
    return()
endif ()

set(GLEW_LIBRARY libGLEW.so)
set (GLFW_STATIC FALSE CACHE BOOL "link against static or dynamic glfw3 library")

find_package(PkgConfig REQUIRED)
    pkg_search_module(GLFW REQUIRED glfw3)
    if (GLFW_STATIC)
        set (GLFW_LIBS ${GLFW_STATIC_LIBRARIES})
    else(GLFW_STATIC)
        set (GLFW_LIBS ${GLFW_LIBRARIES})
    endif(GLFW_STATIC)

include_directories(include ./../OpenGL/include)
#include_directories(SYSTEM ${OpenCV_INCLUDE_DIRS}  ${OpenGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS})
include_directories(SYSTEM ${OpenCV_INCLUDE_DIRS} ${GLEW_INCLUDE_DIR} ${GLFW_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR})

################################################################################
# add the executable
################################################################################

#SET( CMAKE_EXE_LINKER_FLAGS   "-L/usr/local/lib")
add_executable(Time_Terminal ${SOURCES})
#-lglfw3 -lGLU -lGL
target_link_libraries(Time_Terminal ${OpenCV_LIBS} ${GLUT_LIBRARIES} ${OPENGL_LIBRARIES} ${GLEW_LIBRARY} ${GLFW_LIBS})
