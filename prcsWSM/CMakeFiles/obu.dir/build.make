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
CMAKE_SOURCE_DIR = /media/sf_Source/Xavier/apps/WSM

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /media/sf_Source/Xavier/apps/WSM

# Include any dependencies generated for this target.
include CMakeFiles/obu.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/obu.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/obu.dir/flags.make

CMakeFiles/obu.dir/src/v2x-obu.c.o: CMakeFiles/obu.dir/flags.make
CMakeFiles/obu.dir/src/v2x-obu.c.o: src/v2x-obu.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/media/sf_Source/Xavier/apps/WSM/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/obu.dir/src/v2x-obu.c.o"
	arm-linux-gnueabi-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/obu.dir/src/v2x-obu.c.o   -c /media/sf_Source/Xavier/apps/WSM/src/v2x-obu.c

CMakeFiles/obu.dir/src/v2x-obu.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/obu.dir/src/v2x-obu.c.i"
	arm-linux-gnueabi-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /media/sf_Source/Xavier/apps/WSM/src/v2x-obu.c > CMakeFiles/obu.dir/src/v2x-obu.c.i

CMakeFiles/obu.dir/src/v2x-obu.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/obu.dir/src/v2x-obu.c.s"
	arm-linux-gnueabi-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /media/sf_Source/Xavier/apps/WSM/src/v2x-obu.c -o CMakeFiles/obu.dir/src/v2x-obu.c.s

CMakeFiles/obu.dir/src/v2x-obu-libdot3.c.o: CMakeFiles/obu.dir/flags.make
CMakeFiles/obu.dir/src/v2x-obu-libdot3.c.o: src/v2x-obu-libdot3.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/media/sf_Source/Xavier/apps/WSM/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/obu.dir/src/v2x-obu-libdot3.c.o"
	arm-linux-gnueabi-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/obu.dir/src/v2x-obu-libdot3.c.o   -c /media/sf_Source/Xavier/apps/WSM/src/v2x-obu-libdot3.c

CMakeFiles/obu.dir/src/v2x-obu-libdot3.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/obu.dir/src/v2x-obu-libdot3.c.i"
	arm-linux-gnueabi-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /media/sf_Source/Xavier/apps/WSM/src/v2x-obu-libdot3.c > CMakeFiles/obu.dir/src/v2x-obu-libdot3.c.i

CMakeFiles/obu.dir/src/v2x-obu-libdot3.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/obu.dir/src/v2x-obu-libdot3.c.s"
	arm-linux-gnueabi-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /media/sf_Source/Xavier/apps/WSM/src/v2x-obu-libdot3.c -o CMakeFiles/obu.dir/src/v2x-obu-libdot3.c.s

CMakeFiles/obu.dir/src/v2x-obu-libwlanaccess.c.o: CMakeFiles/obu.dir/flags.make
CMakeFiles/obu.dir/src/v2x-obu-libwlanaccess.c.o: src/v2x-obu-libwlanaccess.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/media/sf_Source/Xavier/apps/WSM/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object CMakeFiles/obu.dir/src/v2x-obu-libwlanaccess.c.o"
	arm-linux-gnueabi-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/obu.dir/src/v2x-obu-libwlanaccess.c.o   -c /media/sf_Source/Xavier/apps/WSM/src/v2x-obu-libwlanaccess.c

CMakeFiles/obu.dir/src/v2x-obu-libwlanaccess.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/obu.dir/src/v2x-obu-libwlanaccess.c.i"
	arm-linux-gnueabi-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /media/sf_Source/Xavier/apps/WSM/src/v2x-obu-libwlanaccess.c > CMakeFiles/obu.dir/src/v2x-obu-libwlanaccess.c.i

CMakeFiles/obu.dir/src/v2x-obu-libwlanaccess.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/obu.dir/src/v2x-obu-libwlanaccess.c.s"
	arm-linux-gnueabi-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /media/sf_Source/Xavier/apps/WSM/src/v2x-obu-libwlanaccess.c -o CMakeFiles/obu.dir/src/v2x-obu-libwlanaccess.c.s

CMakeFiles/obu.dir/src/v2x-obu-rx.c.o: CMakeFiles/obu.dir/flags.make
CMakeFiles/obu.dir/src/v2x-obu-rx.c.o: src/v2x-obu-rx.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/media/sf_Source/Xavier/apps/WSM/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building C object CMakeFiles/obu.dir/src/v2x-obu-rx.c.o"
	arm-linux-gnueabi-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/obu.dir/src/v2x-obu-rx.c.o   -c /media/sf_Source/Xavier/apps/WSM/src/v2x-obu-rx.c

CMakeFiles/obu.dir/src/v2x-obu-rx.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/obu.dir/src/v2x-obu-rx.c.i"
	arm-linux-gnueabi-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /media/sf_Source/Xavier/apps/WSM/src/v2x-obu-rx.c > CMakeFiles/obu.dir/src/v2x-obu-rx.c.i

CMakeFiles/obu.dir/src/v2x-obu-rx.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/obu.dir/src/v2x-obu-rx.c.s"
	arm-linux-gnueabi-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /media/sf_Source/Xavier/apps/WSM/src/v2x-obu-rx.c -o CMakeFiles/obu.dir/src/v2x-obu-rx.c.s

CMakeFiles/obu.dir/src/v2x-obu-tx-wsm.c.o: CMakeFiles/obu.dir/flags.make
CMakeFiles/obu.dir/src/v2x-obu-tx-wsm.c.o: src/v2x-obu-tx-wsm.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/media/sf_Source/Xavier/apps/WSM/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building C object CMakeFiles/obu.dir/src/v2x-obu-tx-wsm.c.o"
	arm-linux-gnueabi-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/obu.dir/src/v2x-obu-tx-wsm.c.o   -c /media/sf_Source/Xavier/apps/WSM/src/v2x-obu-tx-wsm.c

CMakeFiles/obu.dir/src/v2x-obu-tx-wsm.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/obu.dir/src/v2x-obu-tx-wsm.c.i"
	arm-linux-gnueabi-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /media/sf_Source/Xavier/apps/WSM/src/v2x-obu-tx-wsm.c > CMakeFiles/obu.dir/src/v2x-obu-tx-wsm.c.i

CMakeFiles/obu.dir/src/v2x-obu-tx-wsm.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/obu.dir/src/v2x-obu-tx-wsm.c.s"
	arm-linux-gnueabi-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /media/sf_Source/Xavier/apps/WSM/src/v2x-obu-tx-wsm.c -o CMakeFiles/obu.dir/src/v2x-obu-tx-wsm.c.s

# Object files for target obu
obu_OBJECTS = \
"CMakeFiles/obu.dir/src/v2x-obu.c.o" \
"CMakeFiles/obu.dir/src/v2x-obu-libdot3.c.o" \
"CMakeFiles/obu.dir/src/v2x-obu-libwlanaccess.c.o" \
"CMakeFiles/obu.dir/src/v2x-obu-rx.c.o" \
"CMakeFiles/obu.dir/src/v2x-obu-tx-wsm.c.o"

# External object files for target obu
obu_EXTERNAL_OBJECTS =

output/obu: CMakeFiles/obu.dir/src/v2x-obu.c.o
output/obu: CMakeFiles/obu.dir/src/v2x-obu-libdot3.c.o
output/obu: CMakeFiles/obu.dir/src/v2x-obu-libwlanaccess.c.o
output/obu: CMakeFiles/obu.dir/src/v2x-obu-rx.c.o
output/obu: CMakeFiles/obu.dir/src/v2x-obu-tx-wsm.c.o
output/obu: CMakeFiles/obu.dir/build.make
output/obu: CMakeFiles/obu.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/media/sf_Source/Xavier/apps/WSM/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Linking C executable output/obu"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/obu.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/obu.dir/build: output/obu

.PHONY : CMakeFiles/obu.dir/build

CMakeFiles/obu.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/obu.dir/cmake_clean.cmake
.PHONY : CMakeFiles/obu.dir/clean

CMakeFiles/obu.dir/depend:
	cd /media/sf_Source/Xavier/apps/WSM && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /media/sf_Source/Xavier/apps/WSM /media/sf_Source/Xavier/apps/WSM /media/sf_Source/Xavier/apps/WSM /media/sf_Source/Xavier/apps/WSM /media/sf_Source/Xavier/apps/WSM/CMakeFiles/obu.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/obu.dir/depend
