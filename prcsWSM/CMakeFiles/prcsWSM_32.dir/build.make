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
CMAKE_SOURCE_DIR = /media/sf_Source/200104_Application_source/prcsWSM

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /media/sf_Source/200104_Application_source/prcsWSM

# Include any dependencies generated for this target.
include CMakeFiles/prcsWSM_32.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/prcsWSM_32.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/prcsWSM_32.dir/flags.make

CMakeFiles/prcsWSM_32.dir/src/v2x-obu.c.o: CMakeFiles/prcsWSM_32.dir/flags.make
CMakeFiles/prcsWSM_32.dir/src/v2x-obu.c.o: src/v2x-obu.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/media/sf_Source/200104_Application_source/prcsWSM/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/prcsWSM_32.dir/src/v2x-obu.c.o"
	arm-linux-gnueabihf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/prcsWSM_32.dir/src/v2x-obu.c.o   -c /media/sf_Source/200104_Application_source/prcsWSM/src/v2x-obu.c

CMakeFiles/prcsWSM_32.dir/src/v2x-obu.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/prcsWSM_32.dir/src/v2x-obu.c.i"
	arm-linux-gnueabihf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /media/sf_Source/200104_Application_source/prcsWSM/src/v2x-obu.c > CMakeFiles/prcsWSM_32.dir/src/v2x-obu.c.i

CMakeFiles/prcsWSM_32.dir/src/v2x-obu.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/prcsWSM_32.dir/src/v2x-obu.c.s"
	arm-linux-gnueabihf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /media/sf_Source/200104_Application_source/prcsWSM/src/v2x-obu.c -o CMakeFiles/prcsWSM_32.dir/src/v2x-obu.c.s

CMakeFiles/prcsWSM_32.dir/src/v2x-obu-libdot3.c.o: CMakeFiles/prcsWSM_32.dir/flags.make
CMakeFiles/prcsWSM_32.dir/src/v2x-obu-libdot3.c.o: src/v2x-obu-libdot3.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/media/sf_Source/200104_Application_source/prcsWSM/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/prcsWSM_32.dir/src/v2x-obu-libdot3.c.o"
	arm-linux-gnueabihf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/prcsWSM_32.dir/src/v2x-obu-libdot3.c.o   -c /media/sf_Source/200104_Application_source/prcsWSM/src/v2x-obu-libdot3.c

CMakeFiles/prcsWSM_32.dir/src/v2x-obu-libdot3.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/prcsWSM_32.dir/src/v2x-obu-libdot3.c.i"
	arm-linux-gnueabihf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /media/sf_Source/200104_Application_source/prcsWSM/src/v2x-obu-libdot3.c > CMakeFiles/prcsWSM_32.dir/src/v2x-obu-libdot3.c.i

CMakeFiles/prcsWSM_32.dir/src/v2x-obu-libdot3.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/prcsWSM_32.dir/src/v2x-obu-libdot3.c.s"
	arm-linux-gnueabihf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /media/sf_Source/200104_Application_source/prcsWSM/src/v2x-obu-libdot3.c -o CMakeFiles/prcsWSM_32.dir/src/v2x-obu-libdot3.c.s

CMakeFiles/prcsWSM_32.dir/src/v2x-obu-libwlanaccess.c.o: CMakeFiles/prcsWSM_32.dir/flags.make
CMakeFiles/prcsWSM_32.dir/src/v2x-obu-libwlanaccess.c.o: src/v2x-obu-libwlanaccess.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/media/sf_Source/200104_Application_source/prcsWSM/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object CMakeFiles/prcsWSM_32.dir/src/v2x-obu-libwlanaccess.c.o"
	arm-linux-gnueabihf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/prcsWSM_32.dir/src/v2x-obu-libwlanaccess.c.o   -c /media/sf_Source/200104_Application_source/prcsWSM/src/v2x-obu-libwlanaccess.c

CMakeFiles/prcsWSM_32.dir/src/v2x-obu-libwlanaccess.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/prcsWSM_32.dir/src/v2x-obu-libwlanaccess.c.i"
	arm-linux-gnueabihf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /media/sf_Source/200104_Application_source/prcsWSM/src/v2x-obu-libwlanaccess.c > CMakeFiles/prcsWSM_32.dir/src/v2x-obu-libwlanaccess.c.i

CMakeFiles/prcsWSM_32.dir/src/v2x-obu-libwlanaccess.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/prcsWSM_32.dir/src/v2x-obu-libwlanaccess.c.s"
	arm-linux-gnueabihf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /media/sf_Source/200104_Application_source/prcsWSM/src/v2x-obu-libwlanaccess.c -o CMakeFiles/prcsWSM_32.dir/src/v2x-obu-libwlanaccess.c.s

CMakeFiles/prcsWSM_32.dir/src/v2x-obu-rx.c.o: CMakeFiles/prcsWSM_32.dir/flags.make
CMakeFiles/prcsWSM_32.dir/src/v2x-obu-rx.c.o: src/v2x-obu-rx.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/media/sf_Source/200104_Application_source/prcsWSM/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building C object CMakeFiles/prcsWSM_32.dir/src/v2x-obu-rx.c.o"
	arm-linux-gnueabihf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/prcsWSM_32.dir/src/v2x-obu-rx.c.o   -c /media/sf_Source/200104_Application_source/prcsWSM/src/v2x-obu-rx.c

CMakeFiles/prcsWSM_32.dir/src/v2x-obu-rx.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/prcsWSM_32.dir/src/v2x-obu-rx.c.i"
	arm-linux-gnueabihf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /media/sf_Source/200104_Application_source/prcsWSM/src/v2x-obu-rx.c > CMakeFiles/prcsWSM_32.dir/src/v2x-obu-rx.c.i

CMakeFiles/prcsWSM_32.dir/src/v2x-obu-rx.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/prcsWSM_32.dir/src/v2x-obu-rx.c.s"
	arm-linux-gnueabihf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /media/sf_Source/200104_Application_source/prcsWSM/src/v2x-obu-rx.c -o CMakeFiles/prcsWSM_32.dir/src/v2x-obu-rx.c.s

CMakeFiles/prcsWSM_32.dir/src/msgQ.c.o: CMakeFiles/prcsWSM_32.dir/flags.make
CMakeFiles/prcsWSM_32.dir/src/msgQ.c.o: src/msgQ.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/media/sf_Source/200104_Application_source/prcsWSM/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building C object CMakeFiles/prcsWSM_32.dir/src/msgQ.c.o"
	arm-linux-gnueabihf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/prcsWSM_32.dir/src/msgQ.c.o   -c /media/sf_Source/200104_Application_source/prcsWSM/src/msgQ.c

CMakeFiles/prcsWSM_32.dir/src/msgQ.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/prcsWSM_32.dir/src/msgQ.c.i"
	arm-linux-gnueabihf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /media/sf_Source/200104_Application_source/prcsWSM/src/msgQ.c > CMakeFiles/prcsWSM_32.dir/src/msgQ.c.i

CMakeFiles/prcsWSM_32.dir/src/msgQ.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/prcsWSM_32.dir/src/msgQ.c.s"
	arm-linux-gnueabihf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /media/sf_Source/200104_Application_source/prcsWSM/src/msgQ.c -o CMakeFiles/prcsWSM_32.dir/src/msgQ.c.s

CMakeFiles/prcsWSM_32.dir/src/hexdump.c.o: CMakeFiles/prcsWSM_32.dir/flags.make
CMakeFiles/prcsWSM_32.dir/src/hexdump.c.o: src/hexdump.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/media/sf_Source/200104_Application_source/prcsWSM/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building C object CMakeFiles/prcsWSM_32.dir/src/hexdump.c.o"
	arm-linux-gnueabihf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/prcsWSM_32.dir/src/hexdump.c.o   -c /media/sf_Source/200104_Application_source/prcsWSM/src/hexdump.c

CMakeFiles/prcsWSM_32.dir/src/hexdump.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/prcsWSM_32.dir/src/hexdump.c.i"
	arm-linux-gnueabihf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /media/sf_Source/200104_Application_source/prcsWSM/src/hexdump.c > CMakeFiles/prcsWSM_32.dir/src/hexdump.c.i

CMakeFiles/prcsWSM_32.dir/src/hexdump.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/prcsWSM_32.dir/src/hexdump.c.s"
	arm-linux-gnueabihf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /media/sf_Source/200104_Application_source/prcsWSM/src/hexdump.c -o CMakeFiles/prcsWSM_32.dir/src/hexdump.c.s

CMakeFiles/prcsWSM_32.dir/src/options.c.o: CMakeFiles/prcsWSM_32.dir/flags.make
CMakeFiles/prcsWSM_32.dir/src/options.c.o: src/options.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/media/sf_Source/200104_Application_source/prcsWSM/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building C object CMakeFiles/prcsWSM_32.dir/src/options.c.o"
	arm-linux-gnueabihf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/prcsWSM_32.dir/src/options.c.o   -c /media/sf_Source/200104_Application_source/prcsWSM/src/options.c

CMakeFiles/prcsWSM_32.dir/src/options.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/prcsWSM_32.dir/src/options.c.i"
	arm-linux-gnueabihf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /media/sf_Source/200104_Application_source/prcsWSM/src/options.c > CMakeFiles/prcsWSM_32.dir/src/options.c.i

CMakeFiles/prcsWSM_32.dir/src/options.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/prcsWSM_32.dir/src/options.c.s"
	arm-linux-gnueabihf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /media/sf_Source/200104_Application_source/prcsWSM/src/options.c -o CMakeFiles/prcsWSM_32.dir/src/options.c.s

CMakeFiles/prcsWSM_32.dir/src/v2x-obu-tx-wsm.c.o: CMakeFiles/prcsWSM_32.dir/flags.make
CMakeFiles/prcsWSM_32.dir/src/v2x-obu-tx-wsm.c.o: src/v2x-obu-tx-wsm.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/media/sf_Source/200104_Application_source/prcsWSM/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building C object CMakeFiles/prcsWSM_32.dir/src/v2x-obu-tx-wsm.c.o"
	arm-linux-gnueabihf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/prcsWSM_32.dir/src/v2x-obu-tx-wsm.c.o   -c /media/sf_Source/200104_Application_source/prcsWSM/src/v2x-obu-tx-wsm.c

CMakeFiles/prcsWSM_32.dir/src/v2x-obu-tx-wsm.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/prcsWSM_32.dir/src/v2x-obu-tx-wsm.c.i"
	arm-linux-gnueabihf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /media/sf_Source/200104_Application_source/prcsWSM/src/v2x-obu-tx-wsm.c > CMakeFiles/prcsWSM_32.dir/src/v2x-obu-tx-wsm.c.i

CMakeFiles/prcsWSM_32.dir/src/v2x-obu-tx-wsm.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/prcsWSM_32.dir/src/v2x-obu-tx-wsm.c.s"
	arm-linux-gnueabihf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /media/sf_Source/200104_Application_source/prcsWSM/src/v2x-obu-tx-wsm.c -o CMakeFiles/prcsWSM_32.dir/src/v2x-obu-tx-wsm.c.s

# Object files for target prcsWSM_32
prcsWSM_32_OBJECTS = \
"CMakeFiles/prcsWSM_32.dir/src/v2x-obu.c.o" \
"CMakeFiles/prcsWSM_32.dir/src/v2x-obu-libdot3.c.o" \
"CMakeFiles/prcsWSM_32.dir/src/v2x-obu-libwlanaccess.c.o" \
"CMakeFiles/prcsWSM_32.dir/src/v2x-obu-rx.c.o" \
"CMakeFiles/prcsWSM_32.dir/src/msgQ.c.o" \
"CMakeFiles/prcsWSM_32.dir/src/hexdump.c.o" \
"CMakeFiles/prcsWSM_32.dir/src/options.c.o" \
"CMakeFiles/prcsWSM_32.dir/src/v2x-obu-tx-wsm.c.o"

# External object files for target prcsWSM_32
prcsWSM_32_EXTERNAL_OBJECTS =

output/prcsWSM_32: CMakeFiles/prcsWSM_32.dir/src/v2x-obu.c.o
output/prcsWSM_32: CMakeFiles/prcsWSM_32.dir/src/v2x-obu-libdot3.c.o
output/prcsWSM_32: CMakeFiles/prcsWSM_32.dir/src/v2x-obu-libwlanaccess.c.o
output/prcsWSM_32: CMakeFiles/prcsWSM_32.dir/src/v2x-obu-rx.c.o
output/prcsWSM_32: CMakeFiles/prcsWSM_32.dir/src/msgQ.c.o
output/prcsWSM_32: CMakeFiles/prcsWSM_32.dir/src/hexdump.c.o
output/prcsWSM_32: CMakeFiles/prcsWSM_32.dir/src/options.c.o
output/prcsWSM_32: CMakeFiles/prcsWSM_32.dir/src/v2x-obu-tx-wsm.c.o
output/prcsWSM_32: CMakeFiles/prcsWSM_32.dir/build.make
output/prcsWSM_32: CMakeFiles/prcsWSM_32.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/media/sf_Source/200104_Application_source/prcsWSM/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Linking C executable output/prcsWSM_32"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/prcsWSM_32.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/prcsWSM_32.dir/build: output/prcsWSM_32

.PHONY : CMakeFiles/prcsWSM_32.dir/build

CMakeFiles/prcsWSM_32.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/prcsWSM_32.dir/cmake_clean.cmake
.PHONY : CMakeFiles/prcsWSM_32.dir/clean

CMakeFiles/prcsWSM_32.dir/depend:
	cd /media/sf_Source/200104_Application_source/prcsWSM && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /media/sf_Source/200104_Application_source/prcsWSM /media/sf_Source/200104_Application_source/prcsWSM /media/sf_Source/200104_Application_source/prcsWSM /media/sf_Source/200104_Application_source/prcsWSM /media/sf_Source/200104_Application_source/prcsWSM/CMakeFiles/prcsWSM_32.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/prcsWSM_32.dir/depend

