cmake_minimum_required(VERSION 2.8)

SET(PROTO_FILES
    ${CMAKE_SOURCE_DIR}/navitia-proto/type.proto
    ${CMAKE_SOURCE_DIR}/navitia-proto/response.proto
    ${CMAKE_SOURCE_DIR}/navitia-proto/request.proto
)

SET(PROTO_HDRS
    ${CMAKE_CURRENT_BINARY_DIR}/request.pb.h
    ${CMAKE_CURRENT_BINARY_DIR}/response.pb.h
    ${CMAKE_CURRENT_BINARY_DIR}/type.pb.h
)

SET(PROTO_SRCS
    ${CMAKE_CURRENT_BINARY_DIR}/request.pb.cc
    ${CMAKE_CURRENT_BINARY_DIR}/response.pb.cc
    ${CMAKE_CURRENT_BINARY_DIR}/type.pb.cc
)

add_custom_command(OUTPUT ${PROTO_HDRS} ${PROTO_SRCS}
    COMMAND protoc ARGS ${PROTO_FILES} -I ${CMAKE_SOURCE_DIR}/navitia-proto
    --cpp_out=${CMAKE_CURRENT_BINARY_DIR}
    VERBATIM DEPENDS ${PROTO_FILES})

set_source_files_properties(${PROTO_HDRS} ${PROTO_SRCS} PROPERTIES GENERATED TRUE)

add_definitions(-DRAPIDJSON_HAS_STDSTRING)

add_library(libasgard
  metrics.cpp
  mode_costing.cpp
  direct_path_response_builder.cpp
  handler.cpp
  util.cpp
  ${CMAKE_SOURCE_DIR}/utils/zmq.cpp
  ${CMAKE_SOURCE_DIR}/utils/exception.cpp
  ${CMAKE_SOURCE_DIR}/utils/coord_parser.cpp
  ${PROTO_SRCS})

add_executable(asgard asgard.cpp)
target_link_libraries(asgard libasgard config boost_system boost_regex boost_thread boost_filesystem ${BOOST_DEV_LIBS} ${VALHALLA_LIBRARIES} z  curl zmq protobuf prometheus-cpp-core prometheus-cpp-pull ${CURLPP_LIBRARIES}) #TODO do not hardcode lib name

enable_testing()

add_subdirectory(tests)
