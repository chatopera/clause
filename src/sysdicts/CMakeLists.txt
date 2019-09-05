cmake_minimum_required(VERSION 3.2)
project(sysdicts C CXX)
add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)
set(3rd_libs ${JSONCPP_LIBRARY_SHARED})

# thrift rpc serving
add_executable(sysdicts_server main.cpp
                        src/handler.cpp
                        serving/Serving.cpp
                        serving/server_constants.cpp
                        serving/server_types.cpp)

target_include_directories(sysdicts_server PUBLIC
                        ${JSONCPP_INCLUDE_DIRS}
                        ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(sysdicts_server lac ${3rd_libs} event)
set_property(TARGET sysdicts_server APPEND_STRING PROPERTY 
    LINK_FLAGS " -pthread")