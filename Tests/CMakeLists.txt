set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
enable_testing()
option(BUILD_GMOCK "" OFF)
add_subdirectory(External/GoogleTest)
