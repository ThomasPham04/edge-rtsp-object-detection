include(FetchContent)

FetchContent_Declare(
        protobuf-c
        GIT_REPOSITORY https://github.com/protobuf-c/protobuf-c.git
        GIT_TAG v1.5.0 # Can be a tag (yaml-cpp-x.x.x), a commit hash, or a branch name (master)
        GIT_SHALLOW TRUE
)
FetchContent_GetProperties( nats-c)

if(NOT  protobuf-c_POPULATED)
    message(STATUS "Fetching  protobuf-c...")
    FetchContent_Populate(protobuf-c)
    add_subdirectory(${protobuf-c_SOURCE_DIR} ${protobuf-c_BINARY_DIR})
endif()