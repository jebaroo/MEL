macro(mel_test MEL_TEST_NAME)
    project(${MEL_TEST_NAME})
    add_executable(${MEL_TEST_NAME} "test_${MEL_TEST_NAME}.cpp")
    target_link_libraries(${MEL_TEST_NAME} MEL)
    set_target_properties(${MEL_TEST_NAME} PROPERTIES FOLDER "Tests")
endmacro()

mel_test(default)
