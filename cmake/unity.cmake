function(enable_unity_build target_name)
    if(NOT TARGET ${target_name})
        message(FATAL_ERROR "'${target_name}' is not a valid target")
    endif()

    set(_unity_batch_size 8)
    
    set_target_properties(${target_name} PROPERTIES 
        UNITY_BUILD ON
        UNITY_BUILD_MODE BATCH
        UNITY_BUILD_BATCH_SIZE ${_unity_batch_size}
        UNITY_BUILD_UNIQUE_ID LAMBDA_UNITY_ID
        INTERPROCEDURAL_OPTIMIZATION_DEBUG OFF
    )
    
    message(STATUS 
        "Enabled unity build for '${target_name}' "
        "(mode=BATCH, batch_size=${_unity_batch_size})"
    )
endfunction()