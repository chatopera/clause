cmake_minimum_required(VERSION 3.2)
project(regex C CXX)
add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)

add_library(regex STATIC src/pattern.cpp)
target_include_directories(regex PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(regex ${JSONCPP_LIBRARY_SHARED})

# Testcases
enable_testing()
add_executable(regex_test tests/testsuite.cpp
                            tests/tst-pattern.cpp)
set_property(TARGET regex_test APPEND_STRING PROPERTY 
   LINK_FLAGS " -pthread")
target_include_directories(regex_test PUBLIC 
                    ${CMAKE_CURRENT_SOURCE_DIR}
                    ${GTEST_INCLUDE_DIR})
target_link_libraries(regex_test ${GTEST_LIBRARY} proto)
target_link_libraries(regex_test ssl crypto dl)