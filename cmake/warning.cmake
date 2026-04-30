option(ENABLE_WARNINGS_AS_ERRORS "Treat C++ warnings as compiler error" OFF)

function(enable_warnings target_name)
    if(NOT TARGET ${target_name})
        message(FATAL_ERROR "'${target_name}' is not a valid target")
    endif()

    target_compile_options(${target_name} PRIVATE
        # MSVC
        $<$<CXX_COMPILER_ID:MSVC>:/W4>
        $<$<AND:$<BOOL:${ENABLE_WARNINGS_AS_ERRORS}>,$<CXX_COMPILER_ID:MSVC>>:/WX>

        # GNU/Clang
        $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall>
        $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wextra>
        $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wpedantic>
        $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wconversion>
        $<$<AND:$<BOOL:${ENABLE_WARNINGS_AS_ERRORS}>,$<CXX_COMPILER_ID:GNU,Clang,AppleClang>>:-Werror>
    )

    message(STATUS "Warnings enabled for '${target_name}'")
    if(ENABLE_WARNINGS_AS_ERRORS)
        message(STATUS "Warnings as errors enabled for '${target_name}'")
    endif()
endfunction()