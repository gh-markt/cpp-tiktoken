cmake_minimum_required(VERSION 3.1.2)

project(tiktoken LANGUAGES CXX VERSION 0.0.1)
find_package(fmt)


set(CMAKE_CXX_STANDARD 20)
set(INSTALL_TARGETS tiktoken)

include(GNUInstallDirs)

option(CPP_TIKTOKEN_INSTALL "Generate the install target." ON)
option(CPP_TIKTOKEN_TESTING "Enable testing" ON)

set(OPENAPI_SOURCES byte_pair_encoding.cc emdedded_resource_reader.cc modelparams.cc encoding.cc encoding_utils.cc pcre2_regex.cc)
set(OPENAPI_HEADERS byte_pair_encoding.h emdedded_resource_reader.h modelparams.h encoding.h encoding_utils.h pcre2_regex.h)

add_library(tiktoken ${OPENAPI_SOURCES})
set_target_properties(tiktoken PROPERTIES PUBLIC_HEADER "${OPENAPI_HEADERS}")
target_link_libraries(tiktoken pcre2-8 fmt::fmt)
target_include_directories(tiktoken PUBLIC  
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>  
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/tiktoken>  # <prefix>/include/mylib
)

if (NOT CPP_TIKTOKEN_TESTING)
    message(STATUS "Tests off")
else()
    add_subdirectory(ut)
endif()

MESSAGE(STATUS "Copying tokenizers to '${CMAKE_BINARY_DIR}/tokenizers'.")
FILE(COPY o200k_base.tiktoken cl100k_base.tiktoken p50k_base.tiktoken r50k_base.tiktoken tokenizer.model tokenizer_llama3.1.model DESTINATION "${CMAKE_BINARY_DIR}/tokenizers")
MESSAGE(STATUS "Tokenizers copied.")

if (CPP_TIKTOKEN_INSTALL)
    include(CMakePackageConfigHelpers)

    set(CPP_TIKTOKEN_CMAKE_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/tiktoken CACHE STRING "Installation directory for cmake files, a relative path that will be joined with ${CMAKE_INSTALL_PREFIX} or an absolute path.")

    install(TARGETS ${INSTALL_TARGETS}
            EXPORT tiktokenTargets
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/tiktoken")

    install(EXPORT tiktokenTargets
            FILE tiktokenTargets.cmake
            DESTINATION ${CPP_TIKTOKEN_CMAKE_DIR})

    write_basic_package_version_file(
        "tiktokenConfigVersion.cmake"
        VERSION ${tiktoken_VERSION}
        COMPATIBILITY AnyNewerVersion)

    install(FILES "${PROJECT_SOURCE_DIR}/support/cmake/tiktokenConfig.cmake" "${CMAKE_CURRENT_BINARY_DIR}/tiktokenConfigVersion.cmake"
        DESTINATION ${CPP_TIKTOKEN_CMAKE_DIR})
endif ()