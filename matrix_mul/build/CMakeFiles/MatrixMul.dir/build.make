# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

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
CMAKE_SOURCE_DIR = /home/swy/git/opencl/matrix_mul

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/swy/git/opencl/matrix_mul/build

# Include any dependencies generated for this target.
include CMakeFiles/MatrixMul.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/MatrixMul.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/MatrixMul.dir/flags.make

CMakeFiles/MatrixMul.dir/MatrixMul.cpp.o: CMakeFiles/MatrixMul.dir/flags.make
CMakeFiles/MatrixMul.dir/MatrixMul.cpp.o: ../MatrixMul.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/swy/git/opencl/matrix_mul/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/MatrixMul.dir/MatrixMul.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/MatrixMul.dir/MatrixMul.cpp.o -c /home/swy/git/opencl/matrix_mul/MatrixMul.cpp

CMakeFiles/MatrixMul.dir/MatrixMul.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/MatrixMul.dir/MatrixMul.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/swy/git/opencl/matrix_mul/MatrixMul.cpp > CMakeFiles/MatrixMul.dir/MatrixMul.cpp.i

CMakeFiles/MatrixMul.dir/MatrixMul.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/MatrixMul.dir/MatrixMul.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/swy/git/opencl/matrix_mul/MatrixMul.cpp -o CMakeFiles/MatrixMul.dir/MatrixMul.cpp.s

CMakeFiles/MatrixMul.dir/MatrixMul.cpp.o.requires:
.PHONY : CMakeFiles/MatrixMul.dir/MatrixMul.cpp.o.requires

CMakeFiles/MatrixMul.dir/MatrixMul.cpp.o.provides: CMakeFiles/MatrixMul.dir/MatrixMul.cpp.o.requires
	$(MAKE) -f CMakeFiles/MatrixMul.dir/build.make CMakeFiles/MatrixMul.dir/MatrixMul.cpp.o.provides.build
.PHONY : CMakeFiles/MatrixMul.dir/MatrixMul.cpp.o.provides

CMakeFiles/MatrixMul.dir/MatrixMul.cpp.o.provides.build: CMakeFiles/MatrixMul.dir/MatrixMul.cpp.o

# Object files for target MatrixMul
MatrixMul_OBJECTS = \
"CMakeFiles/MatrixMul.dir/MatrixMul.cpp.o"

# External object files for target MatrixMul
MatrixMul_EXTERNAL_OBJECTS =

MatrixMul: CMakeFiles/MatrixMul.dir/MatrixMul.cpp.o
MatrixMul: CMakeFiles/MatrixMul.dir/build.make
MatrixMul: /usr/lib/x86_64-linux-gnu/libOpenCL.so
MatrixMul: CMakeFiles/MatrixMul.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable MatrixMul"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/MatrixMul.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/MatrixMul.dir/build: MatrixMul
.PHONY : CMakeFiles/MatrixMul.dir/build

CMakeFiles/MatrixMul.dir/requires: CMakeFiles/MatrixMul.dir/MatrixMul.cpp.o.requires
.PHONY : CMakeFiles/MatrixMul.dir/requires

CMakeFiles/MatrixMul.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/MatrixMul.dir/cmake_clean.cmake
.PHONY : CMakeFiles/MatrixMul.dir/clean

CMakeFiles/MatrixMul.dir/depend:
	cd /home/swy/git/opencl/matrix_mul/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/swy/git/opencl/matrix_mul /home/swy/git/opencl/matrix_mul /home/swy/git/opencl/matrix_mul/build /home/swy/git/opencl/matrix_mul/build /home/swy/git/opencl/matrix_mul/build/CMakeFiles/MatrixMul.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/MatrixMul.dir/depend
