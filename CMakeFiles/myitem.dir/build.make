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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/nowcoder/my_item

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/nowcoder/my_item

# Include any dependencies generated for this target.
include CMakeFiles/myitem.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/myitem.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/myitem.dir/flags.make

CMakeFiles/myitem.dir/SourceCodes/fd_manager.cc.o: CMakeFiles/myitem.dir/flags.make
CMakeFiles/myitem.dir/SourceCodes/fd_manager.cc.o: SourceCodes/fd_manager.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/nowcoder/my_item/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/myitem.dir/SourceCodes/fd_manager.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/myitem.dir/SourceCodes/fd_manager.cc.o -c /home/nowcoder/my_item/SourceCodes/fd_manager.cc

CMakeFiles/myitem.dir/SourceCodes/fd_manager.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/myitem.dir/SourceCodes/fd_manager.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/nowcoder/my_item/SourceCodes/fd_manager.cc > CMakeFiles/myitem.dir/SourceCodes/fd_manager.cc.i

CMakeFiles/myitem.dir/SourceCodes/fd_manager.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/myitem.dir/SourceCodes/fd_manager.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/nowcoder/my_item/SourceCodes/fd_manager.cc -o CMakeFiles/myitem.dir/SourceCodes/fd_manager.cc.s

CMakeFiles/myitem.dir/SourceCodes/fd_manager.cc.o.requires:

.PHONY : CMakeFiles/myitem.dir/SourceCodes/fd_manager.cc.o.requires

CMakeFiles/myitem.dir/SourceCodes/fd_manager.cc.o.provides: CMakeFiles/myitem.dir/SourceCodes/fd_manager.cc.o.requires
	$(MAKE) -f CMakeFiles/myitem.dir/build.make CMakeFiles/myitem.dir/SourceCodes/fd_manager.cc.o.provides.build
.PHONY : CMakeFiles/myitem.dir/SourceCodes/fd_manager.cc.o.provides

CMakeFiles/myitem.dir/SourceCodes/fd_manager.cc.o.provides.build: CMakeFiles/myitem.dir/SourceCodes/fd_manager.cc.o


CMakeFiles/myitem.dir/SourceCodes/fiber.cc.o: CMakeFiles/myitem.dir/flags.make
CMakeFiles/myitem.dir/SourceCodes/fiber.cc.o: SourceCodes/fiber.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/nowcoder/my_item/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/myitem.dir/SourceCodes/fiber.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/myitem.dir/SourceCodes/fiber.cc.o -c /home/nowcoder/my_item/SourceCodes/fiber.cc

CMakeFiles/myitem.dir/SourceCodes/fiber.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/myitem.dir/SourceCodes/fiber.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/nowcoder/my_item/SourceCodes/fiber.cc > CMakeFiles/myitem.dir/SourceCodes/fiber.cc.i

CMakeFiles/myitem.dir/SourceCodes/fiber.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/myitem.dir/SourceCodes/fiber.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/nowcoder/my_item/SourceCodes/fiber.cc -o CMakeFiles/myitem.dir/SourceCodes/fiber.cc.s

CMakeFiles/myitem.dir/SourceCodes/fiber.cc.o.requires:

.PHONY : CMakeFiles/myitem.dir/SourceCodes/fiber.cc.o.requires

CMakeFiles/myitem.dir/SourceCodes/fiber.cc.o.provides: CMakeFiles/myitem.dir/SourceCodes/fiber.cc.o.requires
	$(MAKE) -f CMakeFiles/myitem.dir/build.make CMakeFiles/myitem.dir/SourceCodes/fiber.cc.o.provides.build
.PHONY : CMakeFiles/myitem.dir/SourceCodes/fiber.cc.o.provides

CMakeFiles/myitem.dir/SourceCodes/fiber.cc.o.provides.build: CMakeFiles/myitem.dir/SourceCodes/fiber.cc.o


CMakeFiles/myitem.dir/SourceCodes/hook.cc.o: CMakeFiles/myitem.dir/flags.make
CMakeFiles/myitem.dir/SourceCodes/hook.cc.o: SourceCodes/hook.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/nowcoder/my_item/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/myitem.dir/SourceCodes/hook.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/myitem.dir/SourceCodes/hook.cc.o -c /home/nowcoder/my_item/SourceCodes/hook.cc

CMakeFiles/myitem.dir/SourceCodes/hook.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/myitem.dir/SourceCodes/hook.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/nowcoder/my_item/SourceCodes/hook.cc > CMakeFiles/myitem.dir/SourceCodes/hook.cc.i

CMakeFiles/myitem.dir/SourceCodes/hook.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/myitem.dir/SourceCodes/hook.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/nowcoder/my_item/SourceCodes/hook.cc -o CMakeFiles/myitem.dir/SourceCodes/hook.cc.s

CMakeFiles/myitem.dir/SourceCodes/hook.cc.o.requires:

.PHONY : CMakeFiles/myitem.dir/SourceCodes/hook.cc.o.requires

CMakeFiles/myitem.dir/SourceCodes/hook.cc.o.provides: CMakeFiles/myitem.dir/SourceCodes/hook.cc.o.requires
	$(MAKE) -f CMakeFiles/myitem.dir/build.make CMakeFiles/myitem.dir/SourceCodes/hook.cc.o.provides.build
.PHONY : CMakeFiles/myitem.dir/SourceCodes/hook.cc.o.provides

CMakeFiles/myitem.dir/SourceCodes/hook.cc.o.provides.build: CMakeFiles/myitem.dir/SourceCodes/hook.cc.o


CMakeFiles/myitem.dir/SourceCodes/iomanager.cc.o: CMakeFiles/myitem.dir/flags.make
CMakeFiles/myitem.dir/SourceCodes/iomanager.cc.o: SourceCodes/iomanager.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/nowcoder/my_item/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/myitem.dir/SourceCodes/iomanager.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/myitem.dir/SourceCodes/iomanager.cc.o -c /home/nowcoder/my_item/SourceCodes/iomanager.cc

CMakeFiles/myitem.dir/SourceCodes/iomanager.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/myitem.dir/SourceCodes/iomanager.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/nowcoder/my_item/SourceCodes/iomanager.cc > CMakeFiles/myitem.dir/SourceCodes/iomanager.cc.i

CMakeFiles/myitem.dir/SourceCodes/iomanager.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/myitem.dir/SourceCodes/iomanager.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/nowcoder/my_item/SourceCodes/iomanager.cc -o CMakeFiles/myitem.dir/SourceCodes/iomanager.cc.s

CMakeFiles/myitem.dir/SourceCodes/iomanager.cc.o.requires:

.PHONY : CMakeFiles/myitem.dir/SourceCodes/iomanager.cc.o.requires

CMakeFiles/myitem.dir/SourceCodes/iomanager.cc.o.provides: CMakeFiles/myitem.dir/SourceCodes/iomanager.cc.o.requires
	$(MAKE) -f CMakeFiles/myitem.dir/build.make CMakeFiles/myitem.dir/SourceCodes/iomanager.cc.o.provides.build
.PHONY : CMakeFiles/myitem.dir/SourceCodes/iomanager.cc.o.provides

CMakeFiles/myitem.dir/SourceCodes/iomanager.cc.o.provides.build: CMakeFiles/myitem.dir/SourceCodes/iomanager.cc.o


CMakeFiles/myitem.dir/SourceCodes/scheduler.cc.o: CMakeFiles/myitem.dir/flags.make
CMakeFiles/myitem.dir/SourceCodes/scheduler.cc.o: SourceCodes/scheduler.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/nowcoder/my_item/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/myitem.dir/SourceCodes/scheduler.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/myitem.dir/SourceCodes/scheduler.cc.o -c /home/nowcoder/my_item/SourceCodes/scheduler.cc

CMakeFiles/myitem.dir/SourceCodes/scheduler.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/myitem.dir/SourceCodes/scheduler.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/nowcoder/my_item/SourceCodes/scheduler.cc > CMakeFiles/myitem.dir/SourceCodes/scheduler.cc.i

CMakeFiles/myitem.dir/SourceCodes/scheduler.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/myitem.dir/SourceCodes/scheduler.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/nowcoder/my_item/SourceCodes/scheduler.cc -o CMakeFiles/myitem.dir/SourceCodes/scheduler.cc.s

CMakeFiles/myitem.dir/SourceCodes/scheduler.cc.o.requires:

.PHONY : CMakeFiles/myitem.dir/SourceCodes/scheduler.cc.o.requires

CMakeFiles/myitem.dir/SourceCodes/scheduler.cc.o.provides: CMakeFiles/myitem.dir/SourceCodes/scheduler.cc.o.requires
	$(MAKE) -f CMakeFiles/myitem.dir/build.make CMakeFiles/myitem.dir/SourceCodes/scheduler.cc.o.provides.build
.PHONY : CMakeFiles/myitem.dir/SourceCodes/scheduler.cc.o.provides

CMakeFiles/myitem.dir/SourceCodes/scheduler.cc.o.provides.build: CMakeFiles/myitem.dir/SourceCodes/scheduler.cc.o


CMakeFiles/myitem.dir/SourceCodes/thread.cc.o: CMakeFiles/myitem.dir/flags.make
CMakeFiles/myitem.dir/SourceCodes/thread.cc.o: SourceCodes/thread.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/nowcoder/my_item/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object CMakeFiles/myitem.dir/SourceCodes/thread.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/myitem.dir/SourceCodes/thread.cc.o -c /home/nowcoder/my_item/SourceCodes/thread.cc

CMakeFiles/myitem.dir/SourceCodes/thread.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/myitem.dir/SourceCodes/thread.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/nowcoder/my_item/SourceCodes/thread.cc > CMakeFiles/myitem.dir/SourceCodes/thread.cc.i

CMakeFiles/myitem.dir/SourceCodes/thread.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/myitem.dir/SourceCodes/thread.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/nowcoder/my_item/SourceCodes/thread.cc -o CMakeFiles/myitem.dir/SourceCodes/thread.cc.s

CMakeFiles/myitem.dir/SourceCodes/thread.cc.o.requires:

.PHONY : CMakeFiles/myitem.dir/SourceCodes/thread.cc.o.requires

CMakeFiles/myitem.dir/SourceCodes/thread.cc.o.provides: CMakeFiles/myitem.dir/SourceCodes/thread.cc.o.requires
	$(MAKE) -f CMakeFiles/myitem.dir/build.make CMakeFiles/myitem.dir/SourceCodes/thread.cc.o.provides.build
.PHONY : CMakeFiles/myitem.dir/SourceCodes/thread.cc.o.provides

CMakeFiles/myitem.dir/SourceCodes/thread.cc.o.provides.build: CMakeFiles/myitem.dir/SourceCodes/thread.cc.o


CMakeFiles/myitem.dir/SourceCodes/timer.cc.o: CMakeFiles/myitem.dir/flags.make
CMakeFiles/myitem.dir/SourceCodes/timer.cc.o: SourceCodes/timer.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/nowcoder/my_item/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building CXX object CMakeFiles/myitem.dir/SourceCodes/timer.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/myitem.dir/SourceCodes/timer.cc.o -c /home/nowcoder/my_item/SourceCodes/timer.cc

CMakeFiles/myitem.dir/SourceCodes/timer.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/myitem.dir/SourceCodes/timer.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/nowcoder/my_item/SourceCodes/timer.cc > CMakeFiles/myitem.dir/SourceCodes/timer.cc.i

CMakeFiles/myitem.dir/SourceCodes/timer.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/myitem.dir/SourceCodes/timer.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/nowcoder/my_item/SourceCodes/timer.cc -o CMakeFiles/myitem.dir/SourceCodes/timer.cc.s

CMakeFiles/myitem.dir/SourceCodes/timer.cc.o.requires:

.PHONY : CMakeFiles/myitem.dir/SourceCodes/timer.cc.o.requires

CMakeFiles/myitem.dir/SourceCodes/timer.cc.o.provides: CMakeFiles/myitem.dir/SourceCodes/timer.cc.o.requires
	$(MAKE) -f CMakeFiles/myitem.dir/build.make CMakeFiles/myitem.dir/SourceCodes/timer.cc.o.provides.build
.PHONY : CMakeFiles/myitem.dir/SourceCodes/timer.cc.o.provides

CMakeFiles/myitem.dir/SourceCodes/timer.cc.o.provides.build: CMakeFiles/myitem.dir/SourceCodes/timer.cc.o


# Object files for target myitem
myitem_OBJECTS = \
"CMakeFiles/myitem.dir/SourceCodes/fd_manager.cc.o" \
"CMakeFiles/myitem.dir/SourceCodes/fiber.cc.o" \
"CMakeFiles/myitem.dir/SourceCodes/hook.cc.o" \
"CMakeFiles/myitem.dir/SourceCodes/iomanager.cc.o" \
"CMakeFiles/myitem.dir/SourceCodes/scheduler.cc.o" \
"CMakeFiles/myitem.dir/SourceCodes/thread.cc.o" \
"CMakeFiles/myitem.dir/SourceCodes/timer.cc.o"

# External object files for target myitem
myitem_EXTERNAL_OBJECTS =

lib/libmyitem.so: CMakeFiles/myitem.dir/SourceCodes/fd_manager.cc.o
lib/libmyitem.so: CMakeFiles/myitem.dir/SourceCodes/fiber.cc.o
lib/libmyitem.so: CMakeFiles/myitem.dir/SourceCodes/hook.cc.o
lib/libmyitem.so: CMakeFiles/myitem.dir/SourceCodes/iomanager.cc.o
lib/libmyitem.so: CMakeFiles/myitem.dir/SourceCodes/scheduler.cc.o
lib/libmyitem.so: CMakeFiles/myitem.dir/SourceCodes/thread.cc.o
lib/libmyitem.so: CMakeFiles/myitem.dir/SourceCodes/timer.cc.o
lib/libmyitem.so: CMakeFiles/myitem.dir/build.make
lib/libmyitem.so: CMakeFiles/myitem.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/nowcoder/my_item/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Linking CXX shared library lib/libmyitem.so"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/myitem.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/myitem.dir/build: lib/libmyitem.so

.PHONY : CMakeFiles/myitem.dir/build

CMakeFiles/myitem.dir/requires: CMakeFiles/myitem.dir/SourceCodes/fd_manager.cc.o.requires
CMakeFiles/myitem.dir/requires: CMakeFiles/myitem.dir/SourceCodes/fiber.cc.o.requires
CMakeFiles/myitem.dir/requires: CMakeFiles/myitem.dir/SourceCodes/hook.cc.o.requires
CMakeFiles/myitem.dir/requires: CMakeFiles/myitem.dir/SourceCodes/iomanager.cc.o.requires
CMakeFiles/myitem.dir/requires: CMakeFiles/myitem.dir/SourceCodes/scheduler.cc.o.requires
CMakeFiles/myitem.dir/requires: CMakeFiles/myitem.dir/SourceCodes/thread.cc.o.requires
CMakeFiles/myitem.dir/requires: CMakeFiles/myitem.dir/SourceCodes/timer.cc.o.requires

.PHONY : CMakeFiles/myitem.dir/requires

CMakeFiles/myitem.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/myitem.dir/cmake_clean.cmake
.PHONY : CMakeFiles/myitem.dir/clean

CMakeFiles/myitem.dir/depend:
	cd /home/nowcoder/my_item && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/nowcoder/my_item /home/nowcoder/my_item /home/nowcoder/my_item /home/nowcoder/my_item /home/nowcoder/my_item/CMakeFiles/myitem.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/myitem.dir/depend

