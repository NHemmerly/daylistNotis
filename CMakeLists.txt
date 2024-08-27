cmake_minimum_required(VERSION 3.22)
project(daylistNotis)

add_executable(daylistNotis src/main.cpp includes/readTxt.h includes/base64/base64.hpp includes/ccpotify.h includes/cppgmail.h)

include(FetchContent)
FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git
                         GIT_TAG 3b15fa82ea74739b574d705fea44959b58142eb8) # Replace with your desired git commit from: https://github.com/libcpr/cpr/releases
FetchContent_MakeAvailable(cpr)
target_link_libraries(daylistNotis PRIVATE cpr::cpr)

include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

target_link_libraries(daylistNotis PRIVATE nlohmann_json::nlohmann_json)

