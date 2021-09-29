
#We use the BOOST_LIBS define is the parent
SET(BOOST_LIBS ${BOOST_LIBS} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

add_executable(direct_path_response_builder_test direct_path_response_builder_test.cpp tile_maker.cpp)
target_link_libraries(direct_path_response_builder_test protobuf ${Boost_LIBRARIES} libasgard ${VALHALLA_LIBRARIES} z curl ${CURLPP_LIBRARIES})
ADD_BOOST_TEST(direct_path_response_builder_test)

add_executable(util_test util_test.cpp)
target_link_libraries(util_test ${Boost_LIBRARIES} libasgard ${VALHALLA_LIBRARIES})
ADD_BOOST_TEST(util_test)

add_executable(projector_test projector_test.cpp tile_maker.cpp)
# Needed to create a directory containing tiles
set_target_properties(projector_test PROPERTIES COMPILE_DEFINITIONS TESTS_BUILD_DIR="${CMAKE_CURRENT_BINARY_DIR}/")
target_link_libraries(projector_test ${Boost_LIBRARIES} protobuf boost_regex libasgard ${VALHALLA_LIBRARIES} z curl)
ADD_BOOST_TEST(projector_test)

add_executable(handler_test handler_test.cpp tile_maker.cpp)
# Needed to create a directory containing tiles
set_target_properties(handler_test PROPERTIES COMPILE_DEFINITIONS TESTS_BUILD_DIR="${CMAKE_CURRENT_BINARY_DIR}/")
target_link_libraries(handler_test ${Boost_LIBRARIES} protobuf boost_regex libasgard ${VALHALLA_LIBRARIES} z curl zmq prometheus-cpp-core prometheus-cpp-pull ${CURLPP_LIBRARIES})
ADD_BOOST_TEST(handler_test)

add_executable(benchmark_projector_cache benchmark_projector_cache.cpp ${CMAKE_SOURCE_DIR}/utils/timer.cpp)
target_link_libraries(benchmark_projector_cache ${Boost_LIBRARIES} libasgard ${VALHALLA_LIBRARIES} boost_program_options protobuf boost_regex z curl)
