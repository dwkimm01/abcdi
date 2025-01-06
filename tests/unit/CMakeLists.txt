
enable_testing()

add_executable(
    abcdi_test
    test_abcdi_01_bool.cpp
    test_abcdi_02_char.cpp
    test_abcdi_03_unsigned_char.cpp
    test_abcdi_04_int16.cpp
    test_abcdi_05_uint16.cpp
    test_abcdi_06_int32.cpp
    test_abcdi_07_uint32.cpp
    test_abcdi_08_int64.cpp
    test_abcdi_09_uint64.cpp
    test_abcdi_10_float.cpp
    test_abcdi_11_double.cpp
    test_abcdi_12_string.cpp
    test_abcdi_13_vector.cpp

    test_abcdi_21_classes.cpp
    test_abcdi_22_multi_classes.cpp
    test_abcdi_23_service_provider.cpp
    test_abcdi_24_binding.cpp
    test_abcdi_25_classes_from_main.cpp
    test_abcdi_26_util.cpp
    test_abcdi_27_string_params.cpp
    test_abcdi_28_more_classes.cpp

    test_abcdi_31_always_create.cpp
)

target_link_libraries(
    abcdi_test
    GTest::gtest_main
)

target_include_directories(abcdi_test PUBLIC ${CMAKE_SOURCE_DIR}/include)

include(GoogleTest)
gtest_discover_tests(abcdi_test)
