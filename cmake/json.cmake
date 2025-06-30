include(FetchContent)

if (MACOS)
    FetchContent_Declare(
            nlohmann
            GIT_REPOSITORY https://github.com/nlohmann/json.git
            GIT_TAG v3.11.3 # Can be a tag (nlohmann-x.x.x), a commit hash, or a branch name (master)
            GIT_SHALLOW TRUE
    )
    FetchContent_GetProperties(nlohmann)

    if(NOT nlohmann_POPULATED)
        message(STATUS "Fetching nlohmann...")
        FetchContent_Populate(nlohmann)
        add_subdirectory(${nlohmann_SOURCE_DIR} ${nlohmann_BINARY_DIR})
    endif()
else()
    find_package(nlohmann_json 3.2.0 REQUIRED)
endif()