#help: https://stackoverflow.com/questions/23626338/c-cmake-make-linker-ld-does-not-find-library-missing-path
cmake_minimum_required(VERSION 3.5.1)
project (Time_Terminal)
project (Lab)
if(("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang") OR ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU") OR (("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel") AND UNIX))
  set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++14 ")
  set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
  set(CMAKE_CXX_FLAGS "-std=c++14 ")
endif()



set(SOURCES1 src/terminal-time.cpp src/base.cpp src/segment.cpp src/interpretation.cpp src/average.cpp src/transferfunction.cpp src/boost.cpp src/reduce.cpp src/boostColor.cpp src/utils.cpp)
set(SOURCES2 src/testlab.cpp       src/base.cpp src/segment.cpp src/interpretation.cpp src/average.cpp src/transferfunction.cpp src/boost.cpp src/reduce.cpp src/boostColor.cpp src/utils.cpp)

################################################################################
# search
################################################################################
find_package(OpenCV REQUIRED)
find_package(OpenMP)

if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

################################################################################
# include dir
################################################################################
include_directories(include)
include_directories(SYSTEM ${OpenCV_INCLUDE_DIRS})

################################################################################
# add the executable
################################################################################

add_executable(Time_Terminal ${SOURCES1})
add_executable(Lab ${SOURCES2})
target_link_libraries(Time_Terminal ${OpenCV_LIBS})
target_link_libraries(Lab ${OpenCV_LIBS})


if(show_time)
  add_definitions(-Dshow_time)
endif()
