include(FetchContent)

# --- GoogleTest ------------------------------------------
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
)

# Required on MSVC to avoid CRT mismatch issues
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)