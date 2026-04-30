add_library(lambdacore STATIC)

enable_unity_build(lambdacore)
enable_warnings(lambdacore)

add_subdirectory(src)

target_include_directories(lambdacore 
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(lambdacore PRIVATE opengl32)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_link_libraries(lambdacore PUBLIC stdc++exp)
    target_compile_options(lambdacore PUBLIC -freflection)
endif()