cmake_minimum_required(VERSION 3.2)
project(clause C CXX)
add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)

set(3rd_libs ${JSONCPP_LIBRARY_SHARED}
             ${LEVELDB_LIBRARY}
             ${MYSQL_LIBRARIES}
             ${XAPIAN_LIBRARY}
             event)

# thrift rpc serving
add_executable(clause_server main.cpp
                        src/maf.hpp
                        src/raf.hpp
                        src/handler.cpp
                        src/mysql.cpp
                        src/activemq.cpp
                        src/subscriber.cpp
                        src/publisher.cpp
                        src/bot.cpp
                        src/sysdicts/client.cpp
                        src/sysdicts/serving/server_constants.cpp
                        src/sysdicts/serving/server_types.cpp
                        src/sysdicts/serving/Serving.cpp
                        serving/Serving.cpp
                        serving/server_constants.cpp
                        serving/server_types.cpp)
target_include_directories(clause_server PUBLIC
                        ${JSONCPP_INCLUDE_DIRS}
                        ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(clause_server PRIVATE 
    ${MYSQL_INCLUDE_DIRS}
    ${LEVELDB_INCLUDE_DIR}
    ${APR_INCLUDE_DIR}
    ${XAPIAN_INCLUDE_DIR}
    ${ACTIVEMQCPP_INCLUDE_DIR})
set_property(TARGET clause_server APPEND_STRING PROPERTY 
    LINK_FLAGS " -pthread")
target_link_libraries(clause_server jieba tsl_hat_trie
                            ner distance redis proto sep 
                            ${3rd_libs})
target_link_libraries(clause_server ${ACTIVEMQCPP_LIBRARIES}
    ${APR_LIBRARY} ssl crypto dl)