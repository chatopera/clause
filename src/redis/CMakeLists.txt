cmake_minimum_required(VERSION 3.2)
project(redis C CXX)
add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)

add_library(redis STATIC src/redis.cpp)
target_include_directories(redis PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(redis PRIVATE hiredis)

# Testcases
enable_testing()
add_executable(redis_test tests/testsuite.cpp
                            tests/tst-redis.cpp)
target_include_directories(redis_test PUBLIC 
                    ${CMAKE_CURRENT_SOURCE_DIR}
                    ${GTEST_INCLUDE_DIR})
set_property(TARGET redis_test APPEND_STRING PROPERTY 
   LINK_FLAGS " -pthread")
target_link_libraries(redis_test ${GTEST_LIBRARY} redis)