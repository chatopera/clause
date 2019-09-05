cmake_minimum_required(VERSION 3.2)
project(proto C CXX)
# http://chen-tao.github.io/2015/11/07/cmakeprotoprotobuf-cpp/
add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)

set(PROTO_DEFS config/intent.proto)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_DEFS})
add_library(proto STATIC ${PROTO_SRCS} ${PROTO_HDRS})
target_link_libraries(proto ${Protobuf_LIBRARIES})
target_include_directories(proto PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
                                        ${Protobuf_INCLUDE_DIR})