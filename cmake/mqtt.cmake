#include(FetchContent)
#FetchContent_Declare(
#paho-mqtt-cpp
#GIT_REPOSITORY https://github.com/eclipse/mosquitto
#GIT_TAG v2.0.18 # Can be a tag (paho-mqtt-x.x.x), a commit hash, or a branch name (master)
#)
#FetchContent_GetProperties(paho-mqtt-cpp)
#include_directories(paho-mqtt-cpp/include)
#if(NOT paho-mqtt-cpp_POPULATED)
#message(STATUS "Fetching paho-mqtt-cpp...")
#FetchContent_Populate(paho-mqtt-cpp)
#add_subdirectory(${paho-mqtt-cpp_SOURCE_DIR} ${paho-mqtt-cpp_BINARY_DIR})
#include_directories(${paho-mqtt-cpp_SOURCE_DIR})
#endif()

MESSAGE(STATUS "#--------------MOSQUITTO---------------------------#")
find_path(MOSQUITTOPP_INCLUDE_DIR mosquittopp.h)
find_library(MOSQUITTOPP_LIBRARY NAMES libmosquittopp mosquittopp)

find_path(MOSQUITTOPP_INCLUDE_DIR mosquittopp.h)
find_library(MOSQUITTOPP_LIBRARY NAMES libmosquittopp mosquittopp)

# set(MOSQUITTOPP_INCLUDE_DIR /usr/local/include/)
# set(MOSQUITTOPP_LIBRARY /usr/local/lib/libmosquittopp.dylib)

message(STATUS "MOSQUITTOPP_INCLUDE_DIR: ${MOSQUITTOPP_INCLUDE_DIR}")
message(STATUS "MOSQUITTOPP_LIBRARY: ${MOSQUITTOPP_LIBRARY}")

