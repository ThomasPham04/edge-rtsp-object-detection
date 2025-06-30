#include(FetchContent)
#
#FetchContent_Declare(
#        yaml-cpp
#        GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
#        GIT_TAG 0.8.0 # Can be a tag (yaml-cpp-x.x.x), a commit hash, or a branch name (master)
#)
#FetchContent_GetProperties(yaml-cpp)
#
#if(NOT yaml-cpp_POPULATED)
#    message(STATUS "Fetching yaml-cpp...")
#    FetchContent_Populate(yaml-cpp)
#    add_subdirectory(${yaml-cpp_SOURCE_DIR} ${yaml-cpp_BINARY_DIR})
#endif()
find_package(yaml-cpp REQUIRED)
message(YAML_CPP_INCLUDE_DIR ${YAML_CPP_INCLUDE_DIR})
include_directories(${YAML_CPP_INCLUDE_DIR})
message(YAML_CPP_LIBRARIES ${YAML_CPP_LIBRARIES})