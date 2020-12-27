file(GLOB_RECURSE COMPILER_SOURCE CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp)
add_library(WaveCompiler STATIC ${COMPILER_SOURCE})

target_include_directories(WaveCompiler PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Source/)

target_compile_features(WaveCompiler PUBLIC cxx_std_17)
set_target_properties(WaveCompiler PROPERTIES CXX_EXTENSIONS OFF)
