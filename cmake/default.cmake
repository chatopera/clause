# some find modules cmake files out of box
# cmake --help-module-list 

find_package(ZLIB REQUIRED)
include_directories(${ZLIB_INCLUDE_DIR})
link_libraries(${ZLIB_STATIC_LIB})

# -lpthread
find_package(Threads REQUIRED)
include_directories(${THREADS_INCLUDE_DIR})
link_libraries(${THREADS_LIBRARY})

# Google Flags
find_package(GFlags REQUIRED)
include_directories(${GFLAGS_INCLUDE_DIR})
link_libraries(${GFLAGS_LIBRARY})

# Google Glog
find_package(Glog REQUIRED)
include_directories(${GLOG_INCLUDE_DIR})
link_libraries(${GLOG_LIBRARY})

# Gperftools
find_package(Gperftools REQUIRED)
include_directories(${GPERFTOOLS_INCLUDE_DIR})
link_libraries(${GPERFTOOLS_LIBRARIES})

# Google Test
set(GTEST_ROOT ${PROJECT_SOURCE_DIR} CACHE PATH "Path to googletest")
find_package(GoogleTest REQUIRED)
## Note: should better link testing module by each target specific
## include_directories(${GTEST_INCLUDE_DIR})
## link_libraries(${GTEST_LIBRARY})

# Protobuf
## https://cmake.org/cmake/help/latest/module/FindProtobuf.html
find_package(Protobuf REQUIRED)
include_directories(${Protobuf_INCLUDE_DIR})
link_libraries(${Protobuf_LIBRARY})

# UUID
# http://www.10tiao.com/html/371/201702/2669790574/1.html
find_package(UUID REQUIRED)
include_directories(${UUID_INCLUDE_DIR})
link_libraries(${UUID_LIBRARY})

# Apache Thrift
find_package(Thrift REQUIRED)
include_directories(${THRIFT_INCLUDE_DIR})
link_libraries(${THRIFT_LIBRARIES})

# OpenSource JsonCpp
find_package(JsonCpp REQUIRED)
# include_directories(${JSONCPP_INCLUDE_DIRS})
# link_libraries(${JSONCPP_LIBRARY})

# Boost
## https://cmake.org/cmake/help/v3.8/module/FindBoost.html
set(Boost_USE_STATIC_LIBS true)
find_package(Boost REQUIRED COMPONENTS 
    system
    filesystem
    date_time 
    iostreams
    serialization
    regex)
include_directories(${Boost_INCLUDE_DIRS})
link_libraries(${Boost_LIBRARIES})
MESSAGE(STATUS "Boost_INCLUDE_DIRS -->" ${Boost_INCLUDE_DIRS})
MESSAGE(STATUS "Boost_LIBRARIES -->" ${Boost_LIBRARIES})
MESSAGE(STATUS "Boost_REGEX_LIBRARY -->" ${Boost_REGEX_LIBRARY})

# Linking APR (Apache Portable Runtime)
find_package(APR REQUIRED)

# ActiveMQ
set(ACTIVEMQCPP_ALTLOCATION ${CMAKE_CURRENT_SOURCE_DIR})
find_package(ActiveMQcpp REQUIRED) # include or link by each project individually

# Xapian
find_package(Xapian REQUIRED) # include or link by each project individually

# CRFsuite
find_package(CRFsuite REQUIRED) # include or link by each project individually

# leveldb
find_package(LevelDB REQUIRED) # include or link by each project individually

# baidu paddle
find_package(Paddle REQUIRED) # include or link by each project individually

# MySQL
find_package(MySQL REQUIRED) # include or link by each project individually

# miscs
## print basic info
include(cmake/miscs/001.printinfo.cmake REQUIRED) 
