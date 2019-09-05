PROJECT(jieba)
CMAKE_MINIMUM_REQUIRED (VERSION 2.6)
add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)

add_library(jieba INTERFACE)
target_include_directories(jieba INTERFACE ${PROJECT_SOURCE_DIR}/include)

add_executable(jiebademo ./demo.cpp)
target_link_libraries(jiebademo jieba)

# Testcases
enable_testing()
add_executable(jieba_test tests/testsuite.cpp
                          tests/tst-jieba.cpp
                          tests/tst-pos_tagger_test.cpp
                          tests/tst-segments.cpp
                          tests/tst-trie.cpp
                          tests/tst-unicode.cpp
                          tests/tst-textrank.cpp
                          tests/tst-pre_filter.cpp
                          tests/tst-keyword_extractor.cpp)
target_include_directories(jieba_test PUBLIC 
                    ${GTEST_INCLUDE_DIR})
set_property(TARGET jieba_test APPEND_STRING PROPERTY 
   LINK_FLAGS " -pthread")

target_link_libraries(jieba_test ${GTEST_LIBRARY} jieba)