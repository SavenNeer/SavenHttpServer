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
CMAKE_SOURCE_DIR = /home/ubuntu/beifen_MySever

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/ubuntu/beifen_MySever/build

# Include any dependencies generated for this target.
include CMakeFiles/SavenHttpServer.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/SavenHttpServer.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/SavenHttpServer.dir/flags.make

CMakeFiles/SavenHttpServer.dir/src/saven_httpserver.cpp.o: CMakeFiles/SavenHttpServer.dir/flags.make
CMakeFiles/SavenHttpServer.dir/src/saven_httpserver.cpp.o: ../src/saven_httpserver.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ubuntu/beifen_MySever/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/SavenHttpServer.dir/src/saven_httpserver.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/SavenHttpServer.dir/src/saven_httpserver.cpp.o -c /home/ubuntu/beifen_MySever/src/saven_httpserver.cpp

CMakeFiles/SavenHttpServer.dir/src/saven_httpserver.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/SavenHttpServer.dir/src/saven_httpserver.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ubuntu/beifen_MySever/src/saven_httpserver.cpp > CMakeFiles/SavenHttpServer.dir/src/saven_httpserver.cpp.i

CMakeFiles/SavenHttpServer.dir/src/saven_httpserver.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/SavenHttpServer.dir/src/saven_httpserver.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ubuntu/beifen_MySever/src/saven_httpserver.cpp -o CMakeFiles/SavenHttpServer.dir/src/saven_httpserver.cpp.s

CMakeFiles/SavenHttpServer.dir/src/saven_httpserver.cpp.o.requires:

.PHONY : CMakeFiles/SavenHttpServer.dir/src/saven_httpserver.cpp.o.requires

CMakeFiles/SavenHttpServer.dir/src/saven_httpserver.cpp.o.provides: CMakeFiles/SavenHttpServer.dir/src/saven_httpserver.cpp.o.requires
	$(MAKE) -f CMakeFiles/SavenHttpServer.dir/build.make CMakeFiles/SavenHttpServer.dir/src/saven_httpserver.cpp.o.provides.build
.PHONY : CMakeFiles/SavenHttpServer.dir/src/saven_httpserver.cpp.o.provides

CMakeFiles/SavenHttpServer.dir/src/saven_httpserver.cpp.o.provides.build: CMakeFiles/SavenHttpServer.dir/src/saven_httpserver.cpp.o


# Object files for target SavenHttpServer
SavenHttpServer_OBJECTS = \
"CMakeFiles/SavenHttpServer.dir/src/saven_httpserver.cpp.o"

# External object files for target SavenHttpServer
SavenHttpServer_EXTERNAL_OBJECTS =

libSavenHttpServer.a: CMakeFiles/SavenHttpServer.dir/src/saven_httpserver.cpp.o
libSavenHttpServer.a: CMakeFiles/SavenHttpServer.dir/build.make
libSavenHttpServer.a: CMakeFiles/SavenHttpServer.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/ubuntu/beifen_MySever/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library libSavenHttpServer.a"
	$(CMAKE_COMMAND) -P CMakeFiles/SavenHttpServer.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/SavenHttpServer.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/SavenHttpServer.dir/build: libSavenHttpServer.a

.PHONY : CMakeFiles/SavenHttpServer.dir/build

CMakeFiles/SavenHttpServer.dir/requires: CMakeFiles/SavenHttpServer.dir/src/saven_httpserver.cpp.o.requires

.PHONY : CMakeFiles/SavenHttpServer.dir/requires

CMakeFiles/SavenHttpServer.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/SavenHttpServer.dir/cmake_clean.cmake
.PHONY : CMakeFiles/SavenHttpServer.dir/clean

CMakeFiles/SavenHttpServer.dir/depend:
	cd /home/ubuntu/beifen_MySever/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ubuntu/beifen_MySever /home/ubuntu/beifen_MySever /home/ubuntu/beifen_MySever/build /home/ubuntu/beifen_MySever/build /home/ubuntu/beifen_MySever/build/CMakeFiles/SavenHttpServer.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/SavenHttpServer.dir/depend

