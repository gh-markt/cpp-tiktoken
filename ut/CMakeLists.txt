include("cmake/gtest.cmake")
enable_testing()

add_executable(tests tests.cpp)

target_link_libraries(tests
 PRIVATE
  GTest::gtest  
  GTest::gtest_main
  tiktoken)

include(GoogleTest)
add_test(gtest tests)
FILE(COPY ../o200k_base.tiktoken ../cl100k_base.tiktoken ../p50k_base.tiktoken ../r50k_base.tiktoken ../tokenizer_llama3.1.model DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/tokenizers")
