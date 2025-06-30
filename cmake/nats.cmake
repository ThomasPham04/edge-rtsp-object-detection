include(FetchContent)

FetchContent_Declare(
        nats-c
        GIT_REPOSITORY https://github.com/nats-io/nats.c.git
        GIT_TAG v3.8.0 # Can be a tag (yaml-cpp-x.x.x), a commit hash, or a branch name (master)
        GIT_SHALLOW TRUE
)
FetchContent_GetProperties( nats-c)

if(NOT  nats-c_POPULATED)
    message(STATUS "Fetching  nats-c...")
    FetchContent_Populate(nats-c)
    add_subdirectory(${nats-c_SOURCE_DIR} ${nats-c_BINARY_DIR})
endif()