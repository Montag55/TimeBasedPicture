# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/lucas/Documents/git/VisoProj-TBP/backend/TBPsys

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/lucas/Documents/git/VisoProj-TBP/backend/TBPsys/build

# Include any dependencies generated for this target.
include CMakeFiles/Time_Terminal.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/Time_Terminal.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/Time_Terminal.dir/flags.make

CMakeFiles/Time_Terminal.dir/src/terminal-time.cpp.o: CMakeFiles/Time_Terminal.dir/flags.make
CMakeFiles/Time_Terminal.dir/src/terminal-time.cpp.o: ../src/terminal-time.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/lucas/Documents/git/VisoProj-TBP/backend/TBPsys/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/Time_Terminal.dir/src/terminal-time.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Time_Terminal.dir/src/terminal-time.cpp.o -c /home/lucas/Documents/git/VisoProj-TBP/backend/TBPsys/src/terminal-time.cpp

CMakeFiles/Time_Terminal.dir/src/terminal-time.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Time_Terminal.dir/src/terminal-time.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/lucas/Documents/git/VisoProj-TBP/backend/TBPsys/src/terminal-time.cpp > CMakeFiles/Time_Terminal.dir/src/terminal-time.cpp.i

CMakeFiles/Time_Terminal.dir/src/terminal-time.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Time_Terminal.dir/src/terminal-time.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/lucas/Documents/git/VisoProj-TBP/backend/TBPsys/src/terminal-time.cpp -o CMakeFiles/Time_Terminal.dir/src/terminal-time.cpp.s

CMakeFiles/Time_Terminal.dir/src/terminal-time.cpp.o.requires:

.PHONY : CMakeFiles/Time_Terminal.dir/src/terminal-time.cpp.o.requires

CMakeFiles/Time_Terminal.dir/src/terminal-time.cpp.o.provides: CMakeFiles/Time_Terminal.dir/src/terminal-time.cpp.o.requires
	$(MAKE) -f CMakeFiles/Time_Terminal.dir/build.make CMakeFiles/Time_Terminal.dir/src/terminal-time.cpp.o.provides.build
.PHONY : CMakeFiles/Time_Terminal.dir/src/terminal-time.cpp.o.provides

CMakeFiles/Time_Terminal.dir/src/terminal-time.cpp.o.provides.build: CMakeFiles/Time_Terminal.dir/src/terminal-time.cpp.o


CMakeFiles/Time_Terminal.dir/src/base.cpp.o: CMakeFiles/Time_Terminal.dir/flags.make
CMakeFiles/Time_Terminal.dir/src/base.cpp.o: ../src/base.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/lucas/Documents/git/VisoProj-TBP/backend/TBPsys/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/Time_Terminal.dir/src/base.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Time_Terminal.dir/src/base.cpp.o -c /home/lucas/Documents/git/VisoProj-TBP/backend/TBPsys/src/base.cpp

CMakeFiles/Time_Terminal.dir/src/base.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Time_Terminal.dir/src/base.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/lucas/Documents/git/VisoProj-TBP/backend/TBPsys/src/base.cpp > CMakeFiles/Time_Terminal.dir/src/base.cpp.i

CMakeFiles/Time_Terminal.dir/src/base.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Time_Terminal.dir/src/base.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/lucas/Documents/git/VisoProj-TBP/backend/TBPsys/src/base.cpp -o CMakeFiles/Time_Terminal.dir/src/base.cpp.s

CMakeFiles/Time_Terminal.dir/src/base.cpp.o.requires:

.PHONY : CMakeFiles/Time_Terminal.dir/src/base.cpp.o.requires

CMakeFiles/Time_Terminal.dir/src/base.cpp.o.provides: CMakeFiles/Time_Terminal.dir/src/base.cpp.o.requires
	$(MAKE) -f CMakeFiles/Time_Terminal.dir/build.make CMakeFiles/Time_Terminal.dir/src/base.cpp.o.provides.build
.PHONY : CMakeFiles/Time_Terminal.dir/src/base.cpp.o.provides

CMakeFiles/Time_Terminal.dir/src/base.cpp.o.provides.build: CMakeFiles/Time_Terminal.dir/src/base.cpp.o


CMakeFiles/Time_Terminal.dir/src/segment.cpp.o: CMakeFiles/Time_Terminal.dir/flags.make
CMakeFiles/Time_Terminal.dir/src/segment.cpp.o: ../src/segment.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/lucas/Documents/git/VisoProj-TBP/backend/TBPsys/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/Time_Terminal.dir/src/segment.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Time_Terminal.dir/src/segment.cpp.o -c /home/lucas/Documents/git/VisoProj-TBP/backend/TBPsys/src/segment.cpp

CMakeFiles/Time_Terminal.dir/src/segment.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Time_Terminal.dir/src/segment.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/lucas/Documents/git/VisoProj-TBP/backend/TBPsys/src/segment.cpp > CMakeFiles/Time_Terminal.dir/src/segment.cpp.i

CMakeFiles/Time_Terminal.dir/src/segment.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Time_Terminal.dir/src/segment.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/lucas/Documents/git/VisoProj-TBP/backend/TBPsys/src/segment.cpp -o CMakeFiles/Time_Terminal.dir/src/segment.cpp.s

CMakeFiles/Time_Terminal.dir/src/segment.cpp.o.requires:

.PHONY : CMakeFiles/Time_Terminal.dir/src/segment.cpp.o.requires

CMakeFiles/Time_Terminal.dir/src/segment.cpp.o.provides: CMakeFiles/Time_Terminal.dir/src/segment.cpp.o.requires
	$(MAKE) -f CMakeFiles/Time_Terminal.dir/build.make CMakeFiles/Time_Terminal.dir/src/segment.cpp.o.provides.build
.PHONY : CMakeFiles/Time_Terminal.dir/src/segment.cpp.o.provides

CMakeFiles/Time_Terminal.dir/src/segment.cpp.o.provides.build: CMakeFiles/Time_Terminal.dir/src/segment.cpp.o


CMakeFiles/Time_Terminal.dir/src/interpretation.cpp.o: CMakeFiles/Time_Terminal.dir/flags.make
CMakeFiles/Time_Terminal.dir/src/interpretation.cpp.o: ../src/interpretation.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/lucas/Documents/git/VisoProj-TBP/backend/TBPsys/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/Time_Terminal.dir/src/interpretation.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Time_Terminal.dir/src/interpretation.cpp.o -c /home/lucas/Documents/git/VisoProj-TBP/backend/TBPsys/src/interpretation.cpp

CMakeFiles/Time_Terminal.dir/src/interpretation.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Time_Terminal.dir/src/interpretation.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/lucas/Documents/git/VisoProj-TBP/backend/TBPsys/src/interpretation.cpp > CMakeFiles/Time_Terminal.dir/src/interpretation.cpp.i

CMakeFiles/Time_Terminal.dir/src/interpretation.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Time_Terminal.dir/src/interpretation.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/lucas/Documents/git/VisoProj-TBP/backend/TBPsys/src/interpretation.cpp -o CMakeFiles/Time_Terminal.dir/src/interpretation.cpp.s

CMakeFiles/Time_Terminal.dir/src/interpretation.cpp.o.requires:

.PHONY : CMakeFiles/Time_Terminal.dir/src/interpretation.cpp.o.requires

CMakeFiles/Time_Terminal.dir/src/interpretation.cpp.o.provides: CMakeFiles/Time_Terminal.dir/src/interpretation.cpp.o.requires
	$(MAKE) -f CMakeFiles/Time_Terminal.dir/build.make CMakeFiles/Time_Terminal.dir/src/interpretation.cpp.o.provides.build
.PHONY : CMakeFiles/Time_Terminal.dir/src/interpretation.cpp.o.provides

CMakeFiles/Time_Terminal.dir/src/interpretation.cpp.o.provides.build: CMakeFiles/Time_Terminal.dir/src/interpretation.cpp.o


CMakeFiles/Time_Terminal.dir/src/average.cpp.o: CMakeFiles/Time_Terminal.dir/flags.make
CMakeFiles/Time_Terminal.dir/src/average.cpp.o: ../src/average.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/lucas/Documents/git/VisoProj-TBP/backend/TBPsys/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/Time_Terminal.dir/src/average.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Time_Terminal.dir/src/average.cpp.o -c /home/lucas/Documents/git/VisoProj-TBP/backend/TBPsys/src/average.cpp

CMakeFiles/Time_Terminal.dir/src/average.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Time_Terminal.dir/src/average.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/lucas/Documents/git/VisoProj-TBP/backend/TBPsys/src/average.cpp > CMakeFiles/Time_Terminal.dir/src/average.cpp.i

CMakeFiles/Time_Terminal.dir/src/average.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Time_Terminal.dir/src/average.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/lucas/Documents/git/VisoProj-TBP/backend/TBPsys/src/average.cpp -o CMakeFiles/Time_Terminal.dir/src/average.cpp.s

CMakeFiles/Time_Terminal.dir/src/average.cpp.o.requires:

.PHONY : CMakeFiles/Time_Terminal.dir/src/average.cpp.o.requires

CMakeFiles/Time_Terminal.dir/src/average.cpp.o.provides: CMakeFiles/Time_Terminal.dir/src/average.cpp.o.requires
	$(MAKE) -f CMakeFiles/Time_Terminal.dir/build.make CMakeFiles/Time_Terminal.dir/src/average.cpp.o.provides.build
.PHONY : CMakeFiles/Time_Terminal.dir/src/average.cpp.o.provides

CMakeFiles/Time_Terminal.dir/src/average.cpp.o.provides.build: CMakeFiles/Time_Terminal.dir/src/average.cpp.o


CMakeFiles/Time_Terminal.dir/src/transferfunction.cpp.o: CMakeFiles/Time_Terminal.dir/flags.make
CMakeFiles/Time_Terminal.dir/src/transferfunction.cpp.o: ../src/transferfunction.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/lucas/Documents/git/VisoProj-TBP/backend/TBPsys/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object CMakeFiles/Time_Terminal.dir/src/transferfunction.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Time_Terminal.dir/src/transferfunction.cpp.o -c /home/lucas/Documents/git/VisoProj-TBP/backend/TBPsys/src/transferfunction.cpp

CMakeFiles/Time_Terminal.dir/src/transferfunction.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Time_Terminal.dir/src/transferfunction.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/lucas/Documents/git/VisoProj-TBP/backend/TBPsys/src/transferfunction.cpp > CMakeFiles/Time_Terminal.dir/src/transferfunction.cpp.i

CMakeFiles/Time_Terminal.dir/src/transferfunction.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Time_Terminal.dir/src/transferfunction.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/lucas/Documents/git/VisoProj-TBP/backend/TBPsys/src/transferfunction.cpp -o CMakeFiles/Time_Terminal.dir/src/transferfunction.cpp.s

CMakeFiles/Time_Terminal.dir/src/transferfunction.cpp.o.requires:

.PHONY : CMakeFiles/Time_Terminal.dir/src/transferfunction.cpp.o.requires

CMakeFiles/Time_Terminal.dir/src/transferfunction.cpp.o.provides: CMakeFiles/Time_Terminal.dir/src/transferfunction.cpp.o.requires
	$(MAKE) -f CMakeFiles/Time_Terminal.dir/build.make CMakeFiles/Time_Terminal.dir/src/transferfunction.cpp.o.provides.build
.PHONY : CMakeFiles/Time_Terminal.dir/src/transferfunction.cpp.o.provides

CMakeFiles/Time_Terminal.dir/src/transferfunction.cpp.o.provides.build: CMakeFiles/Time_Terminal.dir/src/transferfunction.cpp.o


CMakeFiles/Time_Terminal.dir/home/lucas/Documents/git/VisoProj-TBP/backend/OpenGL/src/glapplication.cpp.o: CMakeFiles/Time_Terminal.dir/flags.make
CMakeFiles/Time_Terminal.dir/home/lucas/Documents/git/VisoProj-TBP/backend/OpenGL/src/glapplication.cpp.o: /home/lucas/Documents/git/VisoProj-TBP/backend/OpenGL/src/glapplication.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/lucas/Documents/git/VisoProj-TBP/backend/TBPsys/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building CXX object CMakeFiles/Time_Terminal.dir/home/lucas/Documents/git/VisoProj-TBP/backend/OpenGL/src/glapplication.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Time_Terminal.dir/home/lucas/Documents/git/VisoProj-TBP/backend/OpenGL/src/glapplication.cpp.o -c /home/lucas/Documents/git/VisoProj-TBP/backend/OpenGL/src/glapplication.cpp

CMakeFiles/Time_Terminal.dir/home/lucas/Documents/git/VisoProj-TBP/backend/OpenGL/src/glapplication.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Time_Terminal.dir/home/lucas/Documents/git/VisoProj-TBP/backend/OpenGL/src/glapplication.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/lucas/Documents/git/VisoProj-TBP/backend/OpenGL/src/glapplication.cpp > CMakeFiles/Time_Terminal.dir/home/lucas/Documents/git/VisoProj-TBP/backend/OpenGL/src/glapplication.cpp.i

CMakeFiles/Time_Terminal.dir/home/lucas/Documents/git/VisoProj-TBP/backend/OpenGL/src/glapplication.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Time_Terminal.dir/home/lucas/Documents/git/VisoProj-TBP/backend/OpenGL/src/glapplication.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/lucas/Documents/git/VisoProj-TBP/backend/OpenGL/src/glapplication.cpp -o CMakeFiles/Time_Terminal.dir/home/lucas/Documents/git/VisoProj-TBP/backend/OpenGL/src/glapplication.cpp.s

CMakeFiles/Time_Terminal.dir/home/lucas/Documents/git/VisoProj-TBP/backend/OpenGL/src/glapplication.cpp.o.requires:

.PHONY : CMakeFiles/Time_Terminal.dir/home/lucas/Documents/git/VisoProj-TBP/backend/OpenGL/src/glapplication.cpp.o.requires

CMakeFiles/Time_Terminal.dir/home/lucas/Documents/git/VisoProj-TBP/backend/OpenGL/src/glapplication.cpp.o.provides: CMakeFiles/Time_Terminal.dir/home/lucas/Documents/git/VisoProj-TBP/backend/OpenGL/src/glapplication.cpp.o.requires
	$(MAKE) -f CMakeFiles/Time_Terminal.dir/build.make CMakeFiles/Time_Terminal.dir/home/lucas/Documents/git/VisoProj-TBP/backend/OpenGL/src/glapplication.cpp.o.provides.build
.PHONY : CMakeFiles/Time_Terminal.dir/home/lucas/Documents/git/VisoProj-TBP/backend/OpenGL/src/glapplication.cpp.o.provides

CMakeFiles/Time_Terminal.dir/home/lucas/Documents/git/VisoProj-TBP/backend/OpenGL/src/glapplication.cpp.o.provides.build: CMakeFiles/Time_Terminal.dir/home/lucas/Documents/git/VisoProj-TBP/backend/OpenGL/src/glapplication.cpp.o


CMakeFiles/Time_Terminal.dir/home/lucas/Documents/git/VisoProj-TBP/backend/OpenGL/src/window.cpp.o: CMakeFiles/Time_Terminal.dir/flags.make
CMakeFiles/Time_Terminal.dir/home/lucas/Documents/git/VisoProj-TBP/backend/OpenGL/src/window.cpp.o: /home/lucas/Documents/git/VisoProj-TBP/backend/OpenGL/src/window.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/lucas/Documents/git/VisoProj-TBP/backend/TBPsys/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building CXX object CMakeFiles/Time_Terminal.dir/home/lucas/Documents/git/VisoProj-TBP/backend/OpenGL/src/window.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Time_Terminal.dir/home/lucas/Documents/git/VisoProj-TBP/backend/OpenGL/src/window.cpp.o -c /home/lucas/Documents/git/VisoProj-TBP/backend/OpenGL/src/window.cpp

CMakeFiles/Time_Terminal.dir/home/lucas/Documents/git/VisoProj-TBP/backend/OpenGL/src/window.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Time_Terminal.dir/home/lucas/Documents/git/VisoProj-TBP/backend/OpenGL/src/window.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/lucas/Documents/git/VisoProj-TBP/backend/OpenGL/src/window.cpp > CMakeFiles/Time_Terminal.dir/home/lucas/Documents/git/VisoProj-TBP/backend/OpenGL/src/window.cpp.i

CMakeFiles/Time_Terminal.dir/home/lucas/Documents/git/VisoProj-TBP/backend/OpenGL/src/window.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Time_Terminal.dir/home/lucas/Documents/git/VisoProj-TBP/backend/OpenGL/src/window.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/lucas/Documents/git/VisoProj-TBP/backend/OpenGL/src/window.cpp -o CMakeFiles/Time_Terminal.dir/home/lucas/Documents/git/VisoProj-TBP/backend/OpenGL/src/window.cpp.s

CMakeFiles/Time_Terminal.dir/home/lucas/Documents/git/VisoProj-TBP/backend/OpenGL/src/window.cpp.o.requires:

.PHONY : CMakeFiles/Time_Terminal.dir/home/lucas/Documents/git/VisoProj-TBP/backend/OpenGL/src/window.cpp.o.requires

CMakeFiles/Time_Terminal.dir/home/lucas/Documents/git/VisoProj-TBP/backend/OpenGL/src/window.cpp.o.provides: CMakeFiles/Time_Terminal.dir/home/lucas/Documents/git/VisoProj-TBP/backend/OpenGL/src/window.cpp.o.requires
	$(MAKE) -f CMakeFiles/Time_Terminal.dir/build.make CMakeFiles/Time_Terminal.dir/home/lucas/Documents/git/VisoProj-TBP/backend/OpenGL/src/window.cpp.o.provides.build
.PHONY : CMakeFiles/Time_Terminal.dir/home/lucas/Documents/git/VisoProj-TBP/backend/OpenGL/src/window.cpp.o.provides

CMakeFiles/Time_Terminal.dir/home/lucas/Documents/git/VisoProj-TBP/backend/OpenGL/src/window.cpp.o.provides.build: CMakeFiles/Time_Terminal.dir/home/lucas/Documents/git/VisoProj-TBP/backend/OpenGL/src/window.cpp.o


# Object files for target Time_Terminal
Time_Terminal_OBJECTS = \
"CMakeFiles/Time_Terminal.dir/src/terminal-time.cpp.o" \
"CMakeFiles/Time_Terminal.dir/src/base.cpp.o" \
"CMakeFiles/Time_Terminal.dir/src/segment.cpp.o" \
"CMakeFiles/Time_Terminal.dir/src/interpretation.cpp.o" \
"CMakeFiles/Time_Terminal.dir/src/average.cpp.o" \
"CMakeFiles/Time_Terminal.dir/src/transferfunction.cpp.o" \
"CMakeFiles/Time_Terminal.dir/home/lucas/Documents/git/VisoProj-TBP/backend/OpenGL/src/glapplication.cpp.o" \
"CMakeFiles/Time_Terminal.dir/home/lucas/Documents/git/VisoProj-TBP/backend/OpenGL/src/window.cpp.o"

# External object files for target Time_Terminal
Time_Terminal_EXTERNAL_OBJECTS =

Time_Terminal: CMakeFiles/Time_Terminal.dir/src/terminal-time.cpp.o
Time_Terminal: CMakeFiles/Time_Terminal.dir/src/base.cpp.o
Time_Terminal: CMakeFiles/Time_Terminal.dir/src/segment.cpp.o
Time_Terminal: CMakeFiles/Time_Terminal.dir/src/interpretation.cpp.o
Time_Terminal: CMakeFiles/Time_Terminal.dir/src/average.cpp.o
Time_Terminal: CMakeFiles/Time_Terminal.dir/src/transferfunction.cpp.o
Time_Terminal: CMakeFiles/Time_Terminal.dir/home/lucas/Documents/git/VisoProj-TBP/backend/OpenGL/src/glapplication.cpp.o
Time_Terminal: CMakeFiles/Time_Terminal.dir/home/lucas/Documents/git/VisoProj-TBP/backend/OpenGL/src/window.cpp.o
Time_Terminal: CMakeFiles/Time_Terminal.dir/build.make
Time_Terminal: /usr/local/lib/libopencv_dnn.so.3.4.3
Time_Terminal: /usr/local/lib/libopencv_ml.so.3.4.3
Time_Terminal: /usr/local/lib/libopencv_objdetect.so.3.4.3
Time_Terminal: /usr/local/lib/libopencv_shape.so.3.4.3
Time_Terminal: /usr/local/lib/libopencv_stitching.so.3.4.3
Time_Terminal: /usr/local/lib/libopencv_superres.so.3.4.3
Time_Terminal: /usr/local/lib/libopencv_videostab.so.3.4.3
Time_Terminal: /usr/lib/x86_64-linux-gnu/libglut.so
Time_Terminal: /usr/lib/x86_64-linux-gnu/libXi.so
Time_Terminal: /usr/lib/x86_64-linux-gnu/libGL.so
Time_Terminal: /usr/lib/x86_64-linux-gnu/libGLU.so
Time_Terminal: /usr/local/lib/libopencv_calib3d.so.3.4.3
Time_Terminal: /usr/local/lib/libopencv_features2d.so.3.4.3
Time_Terminal: /usr/local/lib/libopencv_flann.so.3.4.3
Time_Terminal: /usr/local/lib/libopencv_highgui.so.3.4.3
Time_Terminal: /usr/local/lib/libopencv_photo.so.3.4.3
Time_Terminal: /usr/local/lib/libopencv_video.so.3.4.3
Time_Terminal: /usr/local/lib/libopencv_videoio.so.3.4.3
Time_Terminal: /usr/local/lib/libopencv_imgcodecs.so.3.4.3
Time_Terminal: /usr/local/lib/libopencv_imgproc.so.3.4.3
Time_Terminal: /usr/local/lib/libopencv_core.so.3.4.3
Time_Terminal: CMakeFiles/Time_Terminal.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/lucas/Documents/git/VisoProj-TBP/backend/TBPsys/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Linking CXX executable Time_Terminal"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Time_Terminal.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/Time_Terminal.dir/build: Time_Terminal

.PHONY : CMakeFiles/Time_Terminal.dir/build

CMakeFiles/Time_Terminal.dir/requires: CMakeFiles/Time_Terminal.dir/src/terminal-time.cpp.o.requires
CMakeFiles/Time_Terminal.dir/requires: CMakeFiles/Time_Terminal.dir/src/base.cpp.o.requires
CMakeFiles/Time_Terminal.dir/requires: CMakeFiles/Time_Terminal.dir/src/segment.cpp.o.requires
CMakeFiles/Time_Terminal.dir/requires: CMakeFiles/Time_Terminal.dir/src/interpretation.cpp.o.requires
CMakeFiles/Time_Terminal.dir/requires: CMakeFiles/Time_Terminal.dir/src/average.cpp.o.requires
CMakeFiles/Time_Terminal.dir/requires: CMakeFiles/Time_Terminal.dir/src/transferfunction.cpp.o.requires
CMakeFiles/Time_Terminal.dir/requires: CMakeFiles/Time_Terminal.dir/home/lucas/Documents/git/VisoProj-TBP/backend/OpenGL/src/glapplication.cpp.o.requires
CMakeFiles/Time_Terminal.dir/requires: CMakeFiles/Time_Terminal.dir/home/lucas/Documents/git/VisoProj-TBP/backend/OpenGL/src/window.cpp.o.requires

.PHONY : CMakeFiles/Time_Terminal.dir/requires

CMakeFiles/Time_Terminal.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/Time_Terminal.dir/cmake_clean.cmake
.PHONY : CMakeFiles/Time_Terminal.dir/clean

CMakeFiles/Time_Terminal.dir/depend:
	cd /home/lucas/Documents/git/VisoProj-TBP/backend/TBPsys/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/lucas/Documents/git/VisoProj-TBP/backend/TBPsys /home/lucas/Documents/git/VisoProj-TBP/backend/TBPsys /home/lucas/Documents/git/VisoProj-TBP/backend/TBPsys/build /home/lucas/Documents/git/VisoProj-TBP/backend/TBPsys/build /home/lucas/Documents/git/VisoProj-TBP/backend/TBPsys/build/CMakeFiles/Time_Terminal.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/Time_Terminal.dir/depend

