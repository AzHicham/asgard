# Third party

# Third party warnings have to be silent, we skip warnings
set(TMP_FLAG ${CMAKE_CXX_FLAGS})
if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_GNUCXX_COMMON_FLAGS} -w")
endif(CMAKE_COMPILER_IS_GNUCXX)

if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CLANG_COMMON_FLAGS} -Wno-everything")
endif("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")

#
# Include third_party code in system mode, to avoid warnings
#
include_directories(SYSTEM "${CMAKE_SOURCE_DIR}/third_party/")
include_directories(SYSTEM "${CMAKE_SOURCE_DIR}/libvalhalla/third_party/rapidjson/include")
include_directories(SYSTEM "${CMAKE_SOURCE_DIR}/third_party/date/include")

#
# prometheus-cpp
#
include_directories(SYSTEM "${CMAKE_SOURCE_DIR}/third_party/prometheus-cpp/core/include/")
include_directories(SYSTEM "${CMAKE_SOURCE_DIR}/third_party/prometheus-cpp/pull/include/")

#
# curlpp
#
include_directories(SYSTEM "${CMAKE_SOURCE_DIR}/third_party/curlpp/include/")
set(CURLPP_LIBRARIES "${CMAKE_BINARY_DIR}/third_party/curlpp/libcurlpp.a")
add_subdirectory(third_party/curlpp)

#prometheus-cpp cmake will refuse to build if the CMAKE_INSTALL_PREFIX is empty
#setting it before will have side effects on how we build packages
set(ENABLE_PUSH OFF CACHE INTERNAL "" FORCE)
add_subdirectory(third_party/prometheus-cpp)

# Reactivate warnings flags
set(CMAKE_CXX_FLAGS ${TMP_FLAG})
