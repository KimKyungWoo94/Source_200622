# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.15

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Produce verbose output by default.
VERBOSE = 1

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
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /media/sf_Source/Xavier/apps/checkGPSD

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /media/sf_Source/Xavier/apps/checkGPSD

# Include any dependencies generated for this target.
include CMakeFiles/checkGPSD.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/checkGPSD.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/checkGPSD.dir/flags.make

CMakeFiles/checkGPSD.dir/shm.c.o: CMakeFiles/checkGPSD.dir/flags.make
CMakeFiles/checkGPSD.dir/shm.c.o: shm.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/media/sf_Source/Xavier/apps/checkGPSD/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/checkGPSD.dir/shm.c.o"
	aarch64-linux-gnu-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/checkGPSD.dir/shm.c.o   -c /media/sf_Source/Xavier/apps/checkGPSD/shm.c

CMakeFiles/checkGPSD.dir/shm.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/checkGPSD.dir/shm.c.i"
	aarch64-linux-gnu-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /media/sf_Source/Xavier/apps/checkGPSD/shm.c > CMakeFiles/checkGPSD.dir/shm.c.i

CMakeFiles/checkGPSD.dir/shm.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/checkGPSD.dir/shm.c.s"
	aarch64-linux-gnu-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /media/sf_Source/Xavier/apps/checkGPSD/shm.c -o CMakeFiles/checkGPSD.dir/shm.c.s

CMakeFiles/checkGPSD.dir/main.c.o: CMakeFiles/checkGPSD.dir/flags.make
CMakeFiles/checkGPSD.dir/main.c.o: main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/media/sf_Source/Xavier/apps/checkGPSD/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/checkGPSD.dir/main.c.o"
	aarch64-linux-gnu-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/checkGPSD.dir/main.c.o   -c /media/sf_Source/Xavier/apps/checkGPSD/main.c

CMakeFiles/checkGPSD.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/checkGPSD.dir/main.c.i"
	aarch64-linux-gnu-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /media/sf_Source/Xavier/apps/checkGPSD/main.c > CMakeFiles/checkGPSD.dir/main.c.i

CMakeFiles/checkGPSD.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/checkGPSD.dir/main.c.s"
	aarch64-linux-gnu-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /media/sf_Source/Xavier/apps/checkGPSD/main.c -o CMakeFiles/checkGPSD.dir/main.c.s

# Object files for target checkGPSD
checkGPSD_OBJECTS = \
"CMakeFiles/checkGPSD.dir/shm.c.o" \
"CMakeFiles/checkGPSD.dir/main.c.o"

# External object files for target checkGPSD
checkGPSD_EXTERNAL_OBJECTS =

output/checkGPSD: CMakeFiles/checkGPSD.dir/shm.c.o
output/checkGPSD: CMakeFiles/checkGPSD.dir/main.c.o
output/checkGPSD: CMakeFiles/checkGPSD.dir/build.make
output/checkGPSD: CMakeFiles/checkGPSD.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/media/sf_Source/Xavier/apps/checkGPSD/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking C executable output/checkGPSD"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/checkGPSD.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/checkGPSD.dir/build: output/checkGPSD

.PHONY : CMakeFiles/checkGPSD.dir/build

CMakeFiles/checkGPSD.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/checkGPSD.dir/cmake_clean.cmake
.PHONY : CMakeFiles/checkGPSD.dir/clean

CMakeFiles/checkGPSD.dir/depend:
	cd /media/sf_Source/Xavier/apps/checkGPSD && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /media/sf_Source/Xavier/apps/checkGPSD /media/sf_Source/Xavier/apps/checkGPSD /media/sf_Source/Xavier/apps/checkGPSD /media/sf_Source/Xavier/apps/checkGPSD /media/sf_Source/Xavier/apps/checkGPSD/CMakeFiles/checkGPSD.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/checkGPSD.dir/depend
