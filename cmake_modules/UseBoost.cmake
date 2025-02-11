if(Boost_VERSION)
    # be silent if already found
    set(Boost_FIND_QUIETLY TRUE)
endif()

set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.71.0 COMPONENTS unit_test_framework thread regex
    serialization date_time filesystem system regex chrono iostreams
    program_options REQUIRED)

link_directories(${Boost_LIBRARY_DIRS})
include_directories("${Boost_INCLUDE_DIRS}")

set(TEST_DEBUG_OUTPUT OFF CACHE BOOL "Shoot test output to stdin")

# Add args for Boost test depending on the version (log_level, etc...)
macro(BUILD_BOOST_TEST_ARGS EXE_NAME)
    if("${Boost_VERSION}" LESS 106200)
        if(TEST_DEBUG_OUTPUT)
            set(BOOST_TEST_ARGS --log_level=all)
        else()
            set(BOOST_TEST_ARGS --log_format=XML --log_sink=results_${EXE_NAME}.xml --log_level=all --report_level=no)
        endif()
    else()
        if(TEST_DEBUG_OUTPUT)
            set(BOOST_TEST_ARGS --logger=HRF,all )
        else()
            set(BOOST_TEST_ARGS --logger=XML,all,results_${EXE_NAME}.xml --report_level=no)
        endif()
    endif()
endmacro(BUILD_BOOST_TEST_ARGS)

macro(ADD_BOOST_TEST EXE_NAME)
    BUILD_BOOST_TEST_ARGS(${EXE_NAME})
    add_test(
        NAME ${EXE_NAME}
        COMMAND "${EXECUTABLE_OUTPUT_PATH}/${EXE_NAME}" ${BOOST_TEST_ARGS} "${ARGN}"
    )
endmacro(ADD_BOOST_TEST)
