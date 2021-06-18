# CMake generated Testfile for 
# Source directory: /__w/asgard/asgard/asgard/tests
# Build directory: /__w/asgard/asgard/asgard/tests
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(direct_path_response_builder_test "/direct_path_response_builder_test" "--log_format=XML" "--log_sink=results_direct_path_response_builder_test.xml" "--log_level=all" "--report_level=no" "")
set_tests_properties(direct_path_response_builder_test PROPERTIES  _BACKTRACE_TRIPLES "/__w/asgard/asgard/cmake_modules/UseBoost.cmake;36;add_test;/__w/asgard/asgard/asgard/tests/CMakeLists.txt;7;ADD_BOOST_TEST;/__w/asgard/asgard/asgard/tests/CMakeLists.txt;0;")
add_test(util_test "/util_test" "--log_format=XML" "--log_sink=results_util_test.xml" "--log_level=all" "--report_level=no" "")
set_tests_properties(util_test PROPERTIES  _BACKTRACE_TRIPLES "/__w/asgard/asgard/cmake_modules/UseBoost.cmake;36;add_test;/__w/asgard/asgard/asgard/tests/CMakeLists.txt;11;ADD_BOOST_TEST;/__w/asgard/asgard/asgard/tests/CMakeLists.txt;0;")
add_test(projector_test "/projector_test" "--log_format=XML" "--log_sink=results_projector_test.xml" "--log_level=all" "--report_level=no" "")
set_tests_properties(projector_test PROPERTIES  _BACKTRACE_TRIPLES "/__w/asgard/asgard/cmake_modules/UseBoost.cmake;36;add_test;/__w/asgard/asgard/asgard/tests/CMakeLists.txt;17;ADD_BOOST_TEST;/__w/asgard/asgard/asgard/tests/CMakeLists.txt;0;")
add_test(handler_test "/handler_test" "--log_format=XML" "--log_sink=results_handler_test.xml" "--log_level=all" "--report_level=no" "")
set_tests_properties(handler_test PROPERTIES  _BACKTRACE_TRIPLES "/__w/asgard/asgard/cmake_modules/UseBoost.cmake;36;add_test;/__w/asgard/asgard/asgard/tests/CMakeLists.txt;23;ADD_BOOST_TEST;/__w/asgard/asgard/asgard/tests/CMakeLists.txt;0;")
