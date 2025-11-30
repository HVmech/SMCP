enable_testing()

function(add_host_test TESTNAME TESTSOURCE)
    add_executable(${TESTNAME} ${TESTSOURCE})
    add_test(NAME ${TESTNAME} COMMAND ${TESTNAME})
endfunction()

add_host_test(run_all_tests main.cpp)