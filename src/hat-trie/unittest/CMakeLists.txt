cmake_minimum_required(VERSION 3.5)

project(tsl_hat_trie_tests)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3 -fpermissive -Werror -Wall -Wextra -Wold-style-cast -O3")
add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)

add_executable(tsl_hat_trie_tests "main.cpp" 
                                  "trie_map_tests.cpp" 
                                  "trie_set_tests.cpp")

# Boost::unit_test_framework
find_package(Boost 1.70.0 REQUIRED COMPONENTS unit_test_framework)
target_link_libraries(tsl_hat_trie_tests PRIVATE boost_unit_test_framework)   

# tsl::hat_trie
add_subdirectory(../ ${CMAKE_CURRENT_BINARY_DIR}/tsl)
target_link_libraries(tsl_hat_trie_tests PRIVATE tsl::hat_trie)  
