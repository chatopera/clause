cmake_minimum_required(VERSION 3.2)
project(lac C CXX)
add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)
set(CMAKE_CXX_FLAGS "-O4 -g -pipe -W -Wall -Wno-unused-parameter -fPIC -fpermissive -std=gnu++11")

add_library(lac src/customization_tagger.cpp
                src/ilac.cpp
                src/lac.cpp
                src/lac_util.cpp
                src/main_tagger.cpp)
target_include_directories(lac PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

message(STATUS "PADDLE_ABS_PATH is set to '${PADDLE_ABS_PATH}'")
target_include_directories(lac BEFORE PRIVATE ${PADDLE_ABS_PATH})
target_include_directories(lac BEFORE PRIVATE ${PADDLE_ABS_PATH}/third_party/install/gflags/include)
target_include_directories(lac BEFORE PRIVATE ${PADDLE_ABS_PATH}/third_party/boost)
target_include_directories(lac BEFORE PRIVATE ${PADDLE_ABS_PATH}/third_party/install/glog/include)
target_include_directories(lac BEFORE PRIVATE ${PADDLE_ABS_PATH}/third_party/install/protobuf/include)
target_include_directories(lac BEFORE PRIVATE ${PADDLE_ABS_PATH}/third_party/eigen3)

set (fluid_lib_path ${PADDLE_ABS_PATH}/paddle/fluid/inference)
if (EXISTS ${fluid_lib_path})
    target_link_directories(lac PRIVATE ${PADDLE_ABS_PATH}/paddle/fluid/inference)
    target_link_libraries(lac ${PADDLE_ABS_PATH}/paddle/fluid/inference/libpaddle_fluid.so)
else ()
    message(FATAL_ERROR "Fluid inference lib not found.\nPlease set -DPADDLE_ROOT=/path/to/your/fluid_inference_lib")
endif ()

# add mkldnn library if it exitsts
set (mkldnn_lib_path ${PADDLE_ABS_PATH}/third_party/install/mkldnn/lib)
if (EXISTS ${mkldnn_lib_path})
    message(STATUS "MKLDNN lib found.")
    target_link_directories(lac PRIVATE ${PADDLE_ABS_PATH}/third_party/install/mkldnn/lib)
    target_link_libraries(lac mkldnn)
else ()
    message(STATUS "MKLDNN lib not found.")
endif()

# add mklml library if it exitsts
set(mklml_inc_path ${PADDLE_ABS_PATH}/third_party/install/mklml/include)
set(mklml_lib_path ${PADDLE_ABS_PATH}/third_party/install/mklml/lib)
if (EXISTS ${mklml_inc_path} AND EXISTS ${mklml_lib_path})
    message(STATUS "MKLML lib found.")   
    target_include_directories(lac PRIVATE ${PADDLE_ABS_PATH}/third_party/install/mklml/include)
    target_link_directories(lac PRIVATE ${PADDLE_ABS_PATH}/third_party/install/mklml/lib)
    target_link_libraries(lac mklml_intel iomp5)
else ()
    message(STATUS "MKLML lib not found.")
endif()

target_link_libraries(lac rt dl pthread)

# Testcases
enable_testing()
add_executable(lac_test tests/testsuite.cpp
                            tests/tst-lac.cpp)
target_include_directories(lac_test PUBLIC
                    ${CMAKE_CURRENT_SOURCE_DIR}
                    ${GTEST_INCLUDE_DIR})

target_link_libraries(lac_test lac ${GTEST_LIBRARY})
set_property(TARGET lac_test APPEND_STRING PROPERTY
   LINK_FLAGS " -pthread")