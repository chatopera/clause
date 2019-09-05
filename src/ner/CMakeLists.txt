cmake_minimum_required(VERSION 3.2)
project(ner C)
add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall")
set(CMAKE_F_FLAGS "${CMAKE_F_FLAGS} -O4 -Wall -fPIC")

add_library(ner STATIC src/iwa.c)
target_link_libraries(ner ${CRFSUITE_LIBRARY} lbfgs)
target_include_directories(ner PUBLIC ${CRFSUITE_INCLUDE_DIR}
                                      ${PROJECT_SOURCE_DIR}/src)

add_executable(crfsuitedemo demo.cpp)
target_link_libraries(crfsuitedemo ner)