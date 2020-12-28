file(GLOB_RECURSE DRIVER_SOURCE CONFIGURE_DEPENDS 
	${CMAKE_CURRENT_SOURCE_DIR}/Source/*.h
	${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp
)
add_executable(wavec ${DRIVER_SOURCE})

target_include_directories(wavec PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Source/)

target_compile_features(wavec PUBLIC cxx_std_17)
set_target_properties(wavec PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(wavec PRIVATE WaveCompiler)
