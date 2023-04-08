cmake_minimum_required(VERSION 3.1.2)

project(tiktoken LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(OPENAPI_SOURCES byte_pair_encoding.cc emdedded_resource_reader.cc modelparams.cc encoding.cc encoding_utils.cc pcre2_regex.cc pcre2_regex.h)

add_library(tiktoken ${OPENAPI_SOURCES})
target_link_libraries(tiktoken pcre2-8)
target_include_directories(tiktoken PRIVATE ${CMAKE_CURRENT_LIST_DIR})
