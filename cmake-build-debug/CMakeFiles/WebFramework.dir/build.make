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
CMAKE_COMMAND = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake

# The command to remove a file.
RM = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/roy/Documents/C++/WebFramework

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/roy/Documents/C++/WebFramework/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/WebFramework.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/WebFramework.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/WebFramework.dir/flags.make

CMakeFiles/WebFramework.dir/components/log.cpp.o: CMakeFiles/WebFramework.dir/flags.make
CMakeFiles/WebFramework.dir/components/log.cpp.o: ../components/log.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/roy/Documents/C++/WebFramework/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/WebFramework.dir/components/log.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/WebFramework.dir/components/log.cpp.o -c /Users/roy/Documents/C++/WebFramework/components/log.cpp

CMakeFiles/WebFramework.dir/components/log.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/WebFramework.dir/components/log.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/roy/Documents/C++/WebFramework/components/log.cpp > CMakeFiles/WebFramework.dir/components/log.cpp.i

CMakeFiles/WebFramework.dir/components/log.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/WebFramework.dir/components/log.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/roy/Documents/C++/WebFramework/components/log.cpp -o CMakeFiles/WebFramework.dir/components/log.cpp.s

CMakeFiles/WebFramework.dir/components/utils.cpp.o: CMakeFiles/WebFramework.dir/flags.make
CMakeFiles/WebFramework.dir/components/utils.cpp.o: ../components/utils.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/roy/Documents/C++/WebFramework/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/WebFramework.dir/components/utils.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/WebFramework.dir/components/utils.cpp.o -c /Users/roy/Documents/C++/WebFramework/components/utils.cpp

CMakeFiles/WebFramework.dir/components/utils.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/WebFramework.dir/components/utils.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/roy/Documents/C++/WebFramework/components/utils.cpp > CMakeFiles/WebFramework.dir/components/utils.cpp.i

CMakeFiles/WebFramework.dir/components/utils.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/WebFramework.dir/components/utils.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/roy/Documents/C++/WebFramework/components/utils.cpp -o CMakeFiles/WebFramework.dir/components/utils.cpp.s

CMakeFiles/WebFramework.dir/components/config.cpp.o: CMakeFiles/WebFramework.dir/flags.make
CMakeFiles/WebFramework.dir/components/config.cpp.o: ../components/config.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/roy/Documents/C++/WebFramework/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/WebFramework.dir/components/config.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/WebFramework.dir/components/config.cpp.o -c /Users/roy/Documents/C++/WebFramework/components/config.cpp

CMakeFiles/WebFramework.dir/components/config.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/WebFramework.dir/components/config.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/roy/Documents/C++/WebFramework/components/config.cpp > CMakeFiles/WebFramework.dir/components/config.cpp.i

CMakeFiles/WebFramework.dir/components/config.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/WebFramework.dir/components/config.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/roy/Documents/C++/WebFramework/components/config.cpp -o CMakeFiles/WebFramework.dir/components/config.cpp.s

CMakeFiles/WebFramework.dir/components/thread.cpp.o: CMakeFiles/WebFramework.dir/flags.make
CMakeFiles/WebFramework.dir/components/thread.cpp.o: ../components/thread.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/roy/Documents/C++/WebFramework/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/WebFramework.dir/components/thread.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/WebFramework.dir/components/thread.cpp.o -c /Users/roy/Documents/C++/WebFramework/components/thread.cpp

CMakeFiles/WebFramework.dir/components/thread.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/WebFramework.dir/components/thread.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/roy/Documents/C++/WebFramework/components/thread.cpp > CMakeFiles/WebFramework.dir/components/thread.cpp.i

CMakeFiles/WebFramework.dir/components/thread.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/WebFramework.dir/components/thread.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/roy/Documents/C++/WebFramework/components/thread.cpp -o CMakeFiles/WebFramework.dir/components/thread.cpp.s

CMakeFiles/WebFramework.dir/components/fiber.cpp.o: CMakeFiles/WebFramework.dir/flags.make
CMakeFiles/WebFramework.dir/components/fiber.cpp.o: ../components/fiber.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/roy/Documents/C++/WebFramework/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/WebFramework.dir/components/fiber.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/WebFramework.dir/components/fiber.cpp.o -c /Users/roy/Documents/C++/WebFramework/components/fiber.cpp

CMakeFiles/WebFramework.dir/components/fiber.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/WebFramework.dir/components/fiber.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/roy/Documents/C++/WebFramework/components/fiber.cpp > CMakeFiles/WebFramework.dir/components/fiber.cpp.i

CMakeFiles/WebFramework.dir/components/fiber.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/WebFramework.dir/components/fiber.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/roy/Documents/C++/WebFramework/components/fiber.cpp -o CMakeFiles/WebFramework.dir/components/fiber.cpp.s

CMakeFiles/WebFramework.dir/components/scheduler.cpp.o: CMakeFiles/WebFramework.dir/flags.make
CMakeFiles/WebFramework.dir/components/scheduler.cpp.o: ../components/scheduler.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/roy/Documents/C++/WebFramework/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object CMakeFiles/WebFramework.dir/components/scheduler.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/WebFramework.dir/components/scheduler.cpp.o -c /Users/roy/Documents/C++/WebFramework/components/scheduler.cpp

CMakeFiles/WebFramework.dir/components/scheduler.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/WebFramework.dir/components/scheduler.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/roy/Documents/C++/WebFramework/components/scheduler.cpp > CMakeFiles/WebFramework.dir/components/scheduler.cpp.i

CMakeFiles/WebFramework.dir/components/scheduler.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/WebFramework.dir/components/scheduler.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/roy/Documents/C++/WebFramework/components/scheduler.cpp -o CMakeFiles/WebFramework.dir/components/scheduler.cpp.s

# Object files for target WebFramework
WebFramework_OBJECTS = \
"CMakeFiles/WebFramework.dir/components/log.cpp.o" \
"CMakeFiles/WebFramework.dir/components/utils.cpp.o" \
"CMakeFiles/WebFramework.dir/components/config.cpp.o" \
"CMakeFiles/WebFramework.dir/components/thread.cpp.o" \
"CMakeFiles/WebFramework.dir/components/fiber.cpp.o" \
"CMakeFiles/WebFramework.dir/components/scheduler.cpp.o"

# External object files for target WebFramework
WebFramework_EXTERNAL_OBJECTS =

libWebFramework.dylib: CMakeFiles/WebFramework.dir/components/log.cpp.o
libWebFramework.dylib: CMakeFiles/WebFramework.dir/components/utils.cpp.o
libWebFramework.dylib: CMakeFiles/WebFramework.dir/components/config.cpp.o
libWebFramework.dylib: CMakeFiles/WebFramework.dir/components/thread.cpp.o
libWebFramework.dylib: CMakeFiles/WebFramework.dir/components/fiber.cpp.o
libWebFramework.dylib: CMakeFiles/WebFramework.dir/components/scheduler.cpp.o
libWebFramework.dylib: CMakeFiles/WebFramework.dir/build.make
libWebFramework.dylib: CMakeFiles/WebFramework.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/roy/Documents/C++/WebFramework/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Linking CXX shared library libWebFramework.dylib"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/WebFramework.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/WebFramework.dir/build: libWebFramework.dylib

.PHONY : CMakeFiles/WebFramework.dir/build

CMakeFiles/WebFramework.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/WebFramework.dir/cmake_clean.cmake
.PHONY : CMakeFiles/WebFramework.dir/clean

CMakeFiles/WebFramework.dir/depend:
	cd /Users/roy/Documents/C++/WebFramework/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/roy/Documents/C++/WebFramework /Users/roy/Documents/C++/WebFramework /Users/roy/Documents/C++/WebFramework/cmake-build-debug /Users/roy/Documents/C++/WebFramework/cmake-build-debug /Users/roy/Documents/C++/WebFramework/cmake-build-debug/CMakeFiles/WebFramework.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/WebFramework.dir/depend

