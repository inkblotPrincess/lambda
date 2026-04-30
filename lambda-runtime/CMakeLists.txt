add_executable(lambda)

enable_unity_build(lambda)
enable_warnings(lambda)

add_subdirectory(src)

target_include_directories(lambda 
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(lambda PRIVATE lambdacore)