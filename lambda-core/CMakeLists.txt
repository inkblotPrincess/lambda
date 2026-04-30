add_library(lambdacore STATIC)

enable_unity_build(lambdacore)
enable_warnings(lambdacore)

add_subdirectory(src)

target_include_directories(lambdacore 
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(lambdacore PRIVATE opengl32)