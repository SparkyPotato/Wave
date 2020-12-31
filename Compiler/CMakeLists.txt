file(GLOB_RECURSE COMPILER_SOURCE CONFIGURE_DEPENDS 
	${CMAKE_CURRENT_SOURCE_DIR}/Include/*.h
	${CMAKE_CURRENT_SOURCE_DIR}/Source/*.h
	${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp
)

option(WAVE_BUILD_SHARED "Build Wave as a shared library" ON)

if (WAVE_BUILD_SHARED)
	add_library(WaveCompiler SHARED ${COMPILER_SOURCE})
else ()
	add_library(WaveCompiler STATIC ${COMPILER_SOURCE})
endif ()

target_include_directories(WaveCompiler PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Include/)
target_include_directories(WaveCompiler PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Include/WaveCompiler)
target_include_directories(WaveCompiler PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Source/)

target_compile_features(WaveCompiler PUBLIC cxx_std_17)
target_compile_definitions(WaveCompiler PRIVATE BUILD_WAVEC)
set_target_properties(WaveCompiler PROPERTIES CXX_EXTENSIONS OFF)
