include(GoogleTest)

add_executable(lambdatests)

enable_unity_build(lambdatests)
enable_warnings(lambdatests)

add_subdirectory(lambda-core)
target_link_libraries(lambdatests PRIVATE lambdacore GTest::gtest_main)

gtest_discover_tests(lambdatests DISCOVERY_MODE PRE_TEST)