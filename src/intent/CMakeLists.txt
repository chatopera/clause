cmake_minimum_required(VERSION 3.2)
project(intent C CXX)
add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)

set(3rd_libs ${JSONCPP_LIBRARY_SHARED}
             ${LEVELDB_LIBRARY}
             ${MYSQL_LIBRARIES}
             ${CRFSUITE_LIBRARY}
             ${XAPIAN_LIBRARY})

# thrift rpc serving
add_executable(intent_server main.cpp
                        src/connection.cpp
                        src/subscriber.cpp
                        src/publisher.cpp
                        src/samples.cpp
                        src/trainer.cpp
                        src/handler.cpp
                        serving/Serving.cpp
                        serving/server_constants.cpp
                        serving/server_types.cpp)
target_include_directories(intent_server PUBLIC
                        ${JSONCPP_INCLUDE_DIRS}
                        ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(intent_server PRIVATE 
    ${XAPIAN_INCLUDE_DIR}
    ${LEVELDB_INCLUDE_DIR}
    ${MYSQL_INCLUDE_DIRS}
    ${CRFSUITE_INCLUDE_DIR}
    ${APR_INCLUDE_DIR}
    ${ACTIVEMQCPP_INCLUDE_DIR})
set_property(TARGET intent_server APPEND_STRING PROPERTY 
    LINK_FLAGS " -pthread")
target_link_libraries(intent_server proto ${3rd_libs} event)
target_link_libraries(intent_server ${ACTIVEMQCPP_LIBRARIES}
    ${APR_LIBRARY} ssl crypto dl)
target_link_libraries(intent_server tsl_hat_trie jieba ner)

# Testcases
enable_testing()
add_executable(intent_test tests/testsuite.cpp
                            tests/tst-activemq.cpp
                            tests/tst-cartprod.cpp
                            tests/tst-train.cpp
                            tests/tst-sysdicts.cpp)
set_property(TARGET intent_test APPEND_STRING PROPERTY 
   LINK_FLAGS " -pthread")
target_include_directories(intent_test PUBLIC 
                    ${CMAKE_CURRENT_SOURCE_DIR}
                    ${GTEST_INCLUDE_DIR})
target_include_directories(intent_test PRIVATE 
    ${APR_INCLUDE_DIR}
    ${ACTIVEMQCPP_INCLUDE_DIR})
target_link_libraries(intent_test ${GTEST_LIBRARY} proto)
target_link_libraries(intent_test ${ACTIVEMQCPP_LIBRARIES}
    ${APR_LIBRARY} ssl crypto dl)
