cmake_minimum_required(VERSION 3.2)
project(sep C)
add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O4 -g -Wall")
set(CMAKE_F_FLAGS "${CMAKE_F_FLAGS} -O4 -Wall -fPIC")

add_library(sep STATIC src/emoji.cpp
                       src/punctuations.cpp
                       src/stopwords.cpp)
target_include_directories(sep PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Testcases
enable_testing()
add_executable(sep_test tests/testsuite.cpp
                            tests/tst-sep.cpp)
target_include_directories(sep_test PUBLIC 
                    ${CMAKE_CURRENT_SOURCE_DIR}
                    ${GTEST_INCLUDE_DIR})
set_property(TARGET sep_test APPEND_STRING PROPERTY 
   LINK_FLAGS " -pthread")
target_link_libraries(sep_test ${GTEST_LIBRARY} sep)