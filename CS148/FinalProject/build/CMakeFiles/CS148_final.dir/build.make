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
CMAKE_SOURCE_DIR = /home/wxia/wei_project/cs231n/CS148/FinalProject

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/wxia/wei_project/cs231n/CS148/FinalProject/build

# Include any dependencies generated for this target.
include CMakeFiles/CS148_final.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/CS148_final.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/CS148_final.dir/flags.make

CMakeFiles/CS148_final.dir/PM.cpp.o: CMakeFiles/CS148_final.dir/flags.make
CMakeFiles/CS148_final.dir/PM.cpp.o: ../PM.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/wxia/wei_project/cs231n/CS148/FinalProject/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/CS148_final.dir/PM.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/CS148_final.dir/PM.cpp.o -c /home/wxia/wei_project/cs231n/CS148/FinalProject/PM.cpp

CMakeFiles/CS148_final.dir/PM.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/CS148_final.dir/PM.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/wxia/wei_project/cs231n/CS148/FinalProject/PM.cpp > CMakeFiles/CS148_final.dir/PM.cpp.i

CMakeFiles/CS148_final.dir/PM.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/CS148_final.dir/PM.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/wxia/wei_project/cs231n/CS148/FinalProject/PM.cpp -o CMakeFiles/CS148_final.dir/PM.cpp.s

CMakeFiles/CS148_final.dir/PM.cpp.o.requires:
.PHONY : CMakeFiles/CS148_final.dir/PM.cpp.o.requires

CMakeFiles/CS148_final.dir/PM.cpp.o.provides: CMakeFiles/CS148_final.dir/PM.cpp.o.requires
	$(MAKE) -f CMakeFiles/CS148_final.dir/build.make CMakeFiles/CS148_final.dir/PM.cpp.o.provides.build
.PHONY : CMakeFiles/CS148_final.dir/PM.cpp.o.provides

CMakeFiles/CS148_final.dir/PM.cpp.o.provides.build: CMakeFiles/CS148_final.dir/PM.cpp.o

CMakeFiles/CS148_final.dir/PlaneTracker.cpp.o: CMakeFiles/CS148_final.dir/flags.make
CMakeFiles/CS148_final.dir/PlaneTracker.cpp.o: ../PlaneTracker.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/wxia/wei_project/cs231n/CS148/FinalProject/build/CMakeFiles $(CMAKE_PROGRESS_2)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/CS148_final.dir/PlaneTracker.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/CS148_final.dir/PlaneTracker.cpp.o -c /home/wxia/wei_project/cs231n/CS148/FinalProject/PlaneTracker.cpp

CMakeFiles/CS148_final.dir/PlaneTracker.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/CS148_final.dir/PlaneTracker.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/wxia/wei_project/cs231n/CS148/FinalProject/PlaneTracker.cpp > CMakeFiles/CS148_final.dir/PlaneTracker.cpp.i

CMakeFiles/CS148_final.dir/PlaneTracker.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/CS148_final.dir/PlaneTracker.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/wxia/wei_project/cs231n/CS148/FinalProject/PlaneTracker.cpp -o CMakeFiles/CS148_final.dir/PlaneTracker.cpp.s

CMakeFiles/CS148_final.dir/PlaneTracker.cpp.o.requires:
.PHONY : CMakeFiles/CS148_final.dir/PlaneTracker.cpp.o.requires

CMakeFiles/CS148_final.dir/PlaneTracker.cpp.o.provides: CMakeFiles/CS148_final.dir/PlaneTracker.cpp.o.requires
	$(MAKE) -f CMakeFiles/CS148_final.dir/build.make CMakeFiles/CS148_final.dir/PlaneTracker.cpp.o.provides.build
.PHONY : CMakeFiles/CS148_final.dir/PlaneTracker.cpp.o.provides

CMakeFiles/CS148_final.dir/PlaneTracker.cpp.o.provides.build: CMakeFiles/CS148_final.dir/PlaneTracker.cpp.o

CMakeFiles/CS148_final.dir/Augmentor.cpp.o: CMakeFiles/CS148_final.dir/flags.make
CMakeFiles/CS148_final.dir/Augmentor.cpp.o: ../Augmentor.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/wxia/wei_project/cs231n/CS148/FinalProject/build/CMakeFiles $(CMAKE_PROGRESS_3)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/CS148_final.dir/Augmentor.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/CS148_final.dir/Augmentor.cpp.o -c /home/wxia/wei_project/cs231n/CS148/FinalProject/Augmentor.cpp

CMakeFiles/CS148_final.dir/Augmentor.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/CS148_final.dir/Augmentor.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/wxia/wei_project/cs231n/CS148/FinalProject/Augmentor.cpp > CMakeFiles/CS148_final.dir/Augmentor.cpp.i

CMakeFiles/CS148_final.dir/Augmentor.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/CS148_final.dir/Augmentor.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/wxia/wei_project/cs231n/CS148/FinalProject/Augmentor.cpp -o CMakeFiles/CS148_final.dir/Augmentor.cpp.s

CMakeFiles/CS148_final.dir/Augmentor.cpp.o.requires:
.PHONY : CMakeFiles/CS148_final.dir/Augmentor.cpp.o.requires

CMakeFiles/CS148_final.dir/Augmentor.cpp.o.provides: CMakeFiles/CS148_final.dir/Augmentor.cpp.o.requires
	$(MAKE) -f CMakeFiles/CS148_final.dir/build.make CMakeFiles/CS148_final.dir/Augmentor.cpp.o.provides.build
.PHONY : CMakeFiles/CS148_final.dir/Augmentor.cpp.o.provides

CMakeFiles/CS148_final.dir/Augmentor.cpp.o.provides.build: CMakeFiles/CS148_final.dir/Augmentor.cpp.o

CMakeFiles/CS148_final.dir/KLTTracker.cpp.o: CMakeFiles/CS148_final.dir/flags.make
CMakeFiles/CS148_final.dir/KLTTracker.cpp.o: ../KLTTracker.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/wxia/wei_project/cs231n/CS148/FinalProject/build/CMakeFiles $(CMAKE_PROGRESS_4)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/CS148_final.dir/KLTTracker.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/CS148_final.dir/KLTTracker.cpp.o -c /home/wxia/wei_project/cs231n/CS148/FinalProject/KLTTracker.cpp

CMakeFiles/CS148_final.dir/KLTTracker.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/CS148_final.dir/KLTTracker.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/wxia/wei_project/cs231n/CS148/FinalProject/KLTTracker.cpp > CMakeFiles/CS148_final.dir/KLTTracker.cpp.i

CMakeFiles/CS148_final.dir/KLTTracker.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/CS148_final.dir/KLTTracker.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/wxia/wei_project/cs231n/CS148/FinalProject/KLTTracker.cpp -o CMakeFiles/CS148_final.dir/KLTTracker.cpp.s

CMakeFiles/CS148_final.dir/KLTTracker.cpp.o.requires:
.PHONY : CMakeFiles/CS148_final.dir/KLTTracker.cpp.o.requires

CMakeFiles/CS148_final.dir/KLTTracker.cpp.o.provides: CMakeFiles/CS148_final.dir/KLTTracker.cpp.o.requires
	$(MAKE) -f CMakeFiles/CS148_final.dir/build.make CMakeFiles/CS148_final.dir/KLTTracker.cpp.o.provides.build
.PHONY : CMakeFiles/CS148_final.dir/KLTTracker.cpp.o.provides

CMakeFiles/CS148_final.dir/KLTTracker.cpp.o.provides.build: CMakeFiles/CS148_final.dir/KLTTracker.cpp.o

CMakeFiles/CS148_final.dir/ORBTracker.cpp.o: CMakeFiles/CS148_final.dir/flags.make
CMakeFiles/CS148_final.dir/ORBTracker.cpp.o: ../ORBTracker.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/wxia/wei_project/cs231n/CS148/FinalProject/build/CMakeFiles $(CMAKE_PROGRESS_5)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/CS148_final.dir/ORBTracker.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/CS148_final.dir/ORBTracker.cpp.o -c /home/wxia/wei_project/cs231n/CS148/FinalProject/ORBTracker.cpp

CMakeFiles/CS148_final.dir/ORBTracker.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/CS148_final.dir/ORBTracker.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/wxia/wei_project/cs231n/CS148/FinalProject/ORBTracker.cpp > CMakeFiles/CS148_final.dir/ORBTracker.cpp.i

CMakeFiles/CS148_final.dir/ORBTracker.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/CS148_final.dir/ORBTracker.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/wxia/wei_project/cs231n/CS148/FinalProject/ORBTracker.cpp -o CMakeFiles/CS148_final.dir/ORBTracker.cpp.s

CMakeFiles/CS148_final.dir/ORBTracker.cpp.o.requires:
.PHONY : CMakeFiles/CS148_final.dir/ORBTracker.cpp.o.requires

CMakeFiles/CS148_final.dir/ORBTracker.cpp.o.provides: CMakeFiles/CS148_final.dir/ORBTracker.cpp.o.requires
	$(MAKE) -f CMakeFiles/CS148_final.dir/build.make CMakeFiles/CS148_final.dir/ORBTracker.cpp.o.provides.build
.PHONY : CMakeFiles/CS148_final.dir/ORBTracker.cpp.o.provides

CMakeFiles/CS148_final.dir/ORBTracker.cpp.o.provides.build: CMakeFiles/CS148_final.dir/ORBTracker.cpp.o

CMakeFiles/CS148_final.dir/CornerDetection.cpp.o: CMakeFiles/CS148_final.dir/flags.make
CMakeFiles/CS148_final.dir/CornerDetection.cpp.o: ../CornerDetection.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/wxia/wei_project/cs231n/CS148/FinalProject/build/CMakeFiles $(CMAKE_PROGRESS_6)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/CS148_final.dir/CornerDetection.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/CS148_final.dir/CornerDetection.cpp.o -c /home/wxia/wei_project/cs231n/CS148/FinalProject/CornerDetection.cpp

CMakeFiles/CS148_final.dir/CornerDetection.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/CS148_final.dir/CornerDetection.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/wxia/wei_project/cs231n/CS148/FinalProject/CornerDetection.cpp > CMakeFiles/CS148_final.dir/CornerDetection.cpp.i

CMakeFiles/CS148_final.dir/CornerDetection.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/CS148_final.dir/CornerDetection.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/wxia/wei_project/cs231n/CS148/FinalProject/CornerDetection.cpp -o CMakeFiles/CS148_final.dir/CornerDetection.cpp.s

CMakeFiles/CS148_final.dir/CornerDetection.cpp.o.requires:
.PHONY : CMakeFiles/CS148_final.dir/CornerDetection.cpp.o.requires

CMakeFiles/CS148_final.dir/CornerDetection.cpp.o.provides: CMakeFiles/CS148_final.dir/CornerDetection.cpp.o.requires
	$(MAKE) -f CMakeFiles/CS148_final.dir/build.make CMakeFiles/CS148_final.dir/CornerDetection.cpp.o.provides.build
.PHONY : CMakeFiles/CS148_final.dir/CornerDetection.cpp.o.provides

CMakeFiles/CS148_final.dir/CornerDetection.cpp.o.provides.build: CMakeFiles/CS148_final.dir/CornerDetection.cpp.o

CMakeFiles/CS148_final.dir/main.cpp.o: CMakeFiles/CS148_final.dir/flags.make
CMakeFiles/CS148_final.dir/main.cpp.o: ../main.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/wxia/wei_project/cs231n/CS148/FinalProject/build/CMakeFiles $(CMAKE_PROGRESS_7)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/CS148_final.dir/main.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/CS148_final.dir/main.cpp.o -c /home/wxia/wei_project/cs231n/CS148/FinalProject/main.cpp

CMakeFiles/CS148_final.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/CS148_final.dir/main.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/wxia/wei_project/cs231n/CS148/FinalProject/main.cpp > CMakeFiles/CS148_final.dir/main.cpp.i

CMakeFiles/CS148_final.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/CS148_final.dir/main.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/wxia/wei_project/cs231n/CS148/FinalProject/main.cpp -o CMakeFiles/CS148_final.dir/main.cpp.s

CMakeFiles/CS148_final.dir/main.cpp.o.requires:
.PHONY : CMakeFiles/CS148_final.dir/main.cpp.o.requires

CMakeFiles/CS148_final.dir/main.cpp.o.provides: CMakeFiles/CS148_final.dir/main.cpp.o.requires
	$(MAKE) -f CMakeFiles/CS148_final.dir/build.make CMakeFiles/CS148_final.dir/main.cpp.o.provides.build
.PHONY : CMakeFiles/CS148_final.dir/main.cpp.o.provides

CMakeFiles/CS148_final.dir/main.cpp.o.provides.build: CMakeFiles/CS148_final.dir/main.cpp.o

# Object files for target CS148_final
CS148_final_OBJECTS = \
"CMakeFiles/CS148_final.dir/PM.cpp.o" \
"CMakeFiles/CS148_final.dir/PlaneTracker.cpp.o" \
"CMakeFiles/CS148_final.dir/Augmentor.cpp.o" \
"CMakeFiles/CS148_final.dir/KLTTracker.cpp.o" \
"CMakeFiles/CS148_final.dir/ORBTracker.cpp.o" \
"CMakeFiles/CS148_final.dir/CornerDetection.cpp.o" \
"CMakeFiles/CS148_final.dir/main.cpp.o"

# External object files for target CS148_final
CS148_final_EXTERNAL_OBJECTS =

CS148_final: CMakeFiles/CS148_final.dir/PM.cpp.o
CS148_final: CMakeFiles/CS148_final.dir/PlaneTracker.cpp.o
CS148_final: CMakeFiles/CS148_final.dir/Augmentor.cpp.o
CS148_final: CMakeFiles/CS148_final.dir/KLTTracker.cpp.o
CS148_final: CMakeFiles/CS148_final.dir/ORBTracker.cpp.o
CS148_final: CMakeFiles/CS148_final.dir/CornerDetection.cpp.o
CS148_final: CMakeFiles/CS148_final.dir/main.cpp.o
CS148_final: CMakeFiles/CS148_final.dir/build.make
CS148_final: /usr/local/lib/libopencv_viz.so.3.1.0
CS148_final: /usr/local/lib/libopencv_videostab.so.3.1.0
CS148_final: /usr/local/lib/libopencv_videoio.so.3.1.0
CS148_final: /usr/local/lib/libopencv_video.so.3.1.0
CS148_final: /usr/local/lib/libopencv_superres.so.3.1.0
CS148_final: /usr/local/lib/libopencv_stitching.so.3.1.0
CS148_final: /usr/local/lib/libopencv_shape.so.3.1.0
CS148_final: /usr/local/lib/libopencv_photo.so.3.1.0
CS148_final: /usr/local/lib/libopencv_objdetect.so.3.1.0
CS148_final: /usr/local/lib/libopencv_ml.so.3.1.0
CS148_final: /usr/local/lib/libopencv_imgproc.so.3.1.0
CS148_final: /usr/local/lib/libopencv_imgcodecs.so.3.1.0
CS148_final: /usr/local/lib/libopencv_highgui.so.3.1.0
CS148_final: /usr/local/lib/libopencv_flann.so.3.1.0
CS148_final: /usr/local/lib/libopencv_features2d.so.3.1.0
CS148_final: /usr/local/lib/libopencv_cudev.so.3.1.0
CS148_final: /usr/local/lib/libopencv_cudawarping.so.3.1.0
CS148_final: /usr/local/lib/libopencv_cudastereo.so.3.1.0
CS148_final: /usr/local/lib/libopencv_cudaoptflow.so.3.1.0
CS148_final: /usr/local/lib/libopencv_cudaobjdetect.so.3.1.0
CS148_final: /usr/local/lib/libopencv_cudalegacy.so.3.1.0
CS148_final: /usr/local/lib/libopencv_cudaimgproc.so.3.1.0
CS148_final: /usr/local/lib/libopencv_cudafilters.so.3.1.0
CS148_final: /usr/local/lib/libopencv_cudafeatures2d.so.3.1.0
CS148_final: /usr/local/lib/libopencv_cudacodec.so.3.1.0
CS148_final: /usr/local/lib/libopencv_cudabgsegm.so.3.1.0
CS148_final: /usr/local/lib/libopencv_cudaarithm.so.3.1.0
CS148_final: /usr/local/lib/libopencv_core.so.3.1.0
CS148_final: /usr/local/lib/libopencv_calib3d.so.3.1.0
CS148_final: /usr/local/lib/libopencv_cudawarping.so.3.1.0
CS148_final: /usr/local/lib/libopencv_objdetect.so.3.1.0
CS148_final: /usr/local/lib/libopencv_cudafilters.so.3.1.0
CS148_final: /usr/local/lib/libopencv_cudaarithm.so.3.1.0
CS148_final: /usr/local/lib/libopencv_features2d.so.3.1.0
CS148_final: /usr/local/lib/libopencv_ml.so.3.1.0
CS148_final: /usr/local/lib/libopencv_highgui.so.3.1.0
CS148_final: /usr/local/lib/libopencv_videoio.so.3.1.0
CS148_final: /usr/local/lib/libopencv_imgcodecs.so.3.1.0
CS148_final: /usr/local/lib/libopencv_flann.so.3.1.0
CS148_final: /usr/local/lib/libopencv_video.so.3.1.0
CS148_final: /usr/local/lib/libopencv_imgproc.so.3.1.0
CS148_final: /usr/local/lib/libopencv_core.so.3.1.0
CS148_final: /usr/local/lib/libopencv_cudev.so.3.1.0
CS148_final: CMakeFiles/CS148_final.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable CS148_final"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/CS148_final.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/CS148_final.dir/build: CS148_final
.PHONY : CMakeFiles/CS148_final.dir/build

CMakeFiles/CS148_final.dir/requires: CMakeFiles/CS148_final.dir/PM.cpp.o.requires
CMakeFiles/CS148_final.dir/requires: CMakeFiles/CS148_final.dir/PlaneTracker.cpp.o.requires
CMakeFiles/CS148_final.dir/requires: CMakeFiles/CS148_final.dir/Augmentor.cpp.o.requires
CMakeFiles/CS148_final.dir/requires: CMakeFiles/CS148_final.dir/KLTTracker.cpp.o.requires
CMakeFiles/CS148_final.dir/requires: CMakeFiles/CS148_final.dir/ORBTracker.cpp.o.requires
CMakeFiles/CS148_final.dir/requires: CMakeFiles/CS148_final.dir/CornerDetection.cpp.o.requires
CMakeFiles/CS148_final.dir/requires: CMakeFiles/CS148_final.dir/main.cpp.o.requires
.PHONY : CMakeFiles/CS148_final.dir/requires

CMakeFiles/CS148_final.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/CS148_final.dir/cmake_clean.cmake
.PHONY : CMakeFiles/CS148_final.dir/clean

CMakeFiles/CS148_final.dir/depend:
	cd /home/wxia/wei_project/cs231n/CS148/FinalProject/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/wxia/wei_project/cs231n/CS148/FinalProject /home/wxia/wei_project/cs231n/CS148/FinalProject /home/wxia/wei_project/cs231n/CS148/FinalProject/build /home/wxia/wei_project/cs231n/CS148/FinalProject/build /home/wxia/wei_project/cs231n/CS148/FinalProject/build/CMakeFiles/CS148_final.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/CS148_final.dir/depend

