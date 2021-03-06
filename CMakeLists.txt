cmake_minimum_required(VERSION 3.12)

project (Wave LANGUAGES CXX)

cmake_policy(SET CMP0079 NEW)

find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
	option(SUBMODULE "Check submodules during build" ON)
	if(SUBMODULE)
		execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
			WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
			RESULT_VARIABLE GIT_SUBMOD_RESULT)
		if(NOT GIT_SUBMOD_RESULT EQUAL "0")
			message(FATAL_ERROR "Updating submodules failed with code ${GIT_SUBMOD_RESULT}.")
		endif()
	endif()
endif()

option(WAVE_BUILD_DOCS "Build the documentation" ON)
option(WAVE_BUILD_TESTS "Build the tests" ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Libraries)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Libraries)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Binaries)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMake" ${CMAKE_MODULE_PATH})

add_subdirectory(Compiler)
add_subdirectory(Driver)

if (WAVE_BUILD_DOCS)
	add_subdirectory(Docs)
endif ()
