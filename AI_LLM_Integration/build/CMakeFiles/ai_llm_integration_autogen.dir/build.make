# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 4.0

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "C:\Program Files\CMake\bin\cmake.exe"

# The command to remove a file.
RM = "C:\Program Files\CMake\bin\cmake.exe" -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = C:\RanOnline_NewBuild\RANOnline_CPP_Ultimate\AI_LLM_Integration

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = C:\RanOnline_NewBuild\RANOnline_CPP_Ultimate\AI_LLM_Integration\build

# Utility rule file for ai_llm_integration_autogen.

# Include any custom commands dependencies for this target.
include CMakeFiles/ai_llm_integration_autogen.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/ai_llm_integration_autogen.dir/progress.make

CMakeFiles/ai_llm_integration_autogen: ai_llm_integration_autogen/timestamp

ai_llm_integration_autogen/timestamp: C:/Qt/6.9.1/mingw_64/bin/moc.exe
ai_llm_integration_autogen/timestamp: C:/Qt/6.9.1/mingw_64/bin/uic.exe
ai_llm_integration_autogen/timestamp: CMakeFiles/ai_llm_integration_autogen.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --blue --bold --progress-dir=C:\RanOnline_NewBuild\RANOnline_CPP_Ultimate\AI_LLM_Integration\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Automatic MOC and UIC for target ai_llm_integration"
	"C:\Program Files\CMake\bin\cmake.exe" -E cmake_autogen C:/RanOnline_NewBuild/RANOnline_CPP_Ultimate/AI_LLM_Integration/build/CMakeFiles/ai_llm_integration_autogen.dir/AutogenInfo.json Release
	"C:\Program Files\CMake\bin\cmake.exe" -E touch C:/RanOnline_NewBuild/RANOnline_CPP_Ultimate/AI_LLM_Integration/build/ai_llm_integration_autogen/timestamp

CMakeFiles/ai_llm_integration_autogen.dir/codegen:
.PHONY : CMakeFiles/ai_llm_integration_autogen.dir/codegen

ai_llm_integration_autogen: CMakeFiles/ai_llm_integration_autogen
ai_llm_integration_autogen: ai_llm_integration_autogen/timestamp
ai_llm_integration_autogen: CMakeFiles/ai_llm_integration_autogen.dir/build.make
.PHONY : ai_llm_integration_autogen

# Rule to build all files generated by this target.
CMakeFiles/ai_llm_integration_autogen.dir/build: ai_llm_integration_autogen
.PHONY : CMakeFiles/ai_llm_integration_autogen.dir/build

CMakeFiles/ai_llm_integration_autogen.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\ai_llm_integration_autogen.dir\cmake_clean.cmake
.PHONY : CMakeFiles/ai_llm_integration_autogen.dir/clean

CMakeFiles/ai_llm_integration_autogen.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" C:\RanOnline_NewBuild\RANOnline_CPP_Ultimate\AI_LLM_Integration C:\RanOnline_NewBuild\RANOnline_CPP_Ultimate\AI_LLM_Integration C:\RanOnline_NewBuild\RANOnline_CPP_Ultimate\AI_LLM_Integration\build C:\RanOnline_NewBuild\RANOnline_CPP_Ultimate\AI_LLM_Integration\build C:\RanOnline_NewBuild\RANOnline_CPP_Ultimate\AI_LLM_Integration\build\CMakeFiles\ai_llm_integration_autogen.dir\DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/ai_llm_integration_autogen.dir/depend

