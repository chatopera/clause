PROJECT(distance)
CMAKE_MINIMUM_REQUIRED (VERSION 2.6)
add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)

add_library(distance STATIC src/similarity.cpp
                            src/levenshtein.cpp
                            src/wordoverlap.cpp)
target_include_directories(distance PUBLIC ${PROJECT_SOURCE_DIR}/src)