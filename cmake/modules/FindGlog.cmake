# - Try to find GLOG
#
# The following variables are optionally searched for defaults
#  GLOG_ROOT_DIR:            Base directory where all GLOG components are found
#
# The following are set after configuration is done:
#  GLOG_FOUND
#  GLOG_INCLUDE_DIRS
#  GLOG_LIBRARIES
#  GLOG_LIBRARYRARY_DIRS

include(FindPackageHandleStandardArgs)

set(GLOG_ROOT_DIR ${PROJECT_SOURCE_DIR} CACHE PATH "Folder contains Gflags")

# We are testing only a couple of files in the include directories
if(WIN32)
    find_path(GLOG_INCLUDE_DIR glog/logging.h
        PATHS ${GLOG_ROOT_DIR}/src/windows)
else()
    find_path(GLOG_INCLUDE_DIR glog/logging.h
        PATHS ${GLOG_ROOT_DIR}/include)
endif()

if(MSVC)
    find_library(GLOG_LIBRARY_RELEASE
        NAMES libglog
        PATHS ${GLOG_ROOT_DIR}
        PATH_SUFFIXES Release)

    find_library(GLOG_LIBRARY_DEBUG
        NAMES libglog-debug
        PATHS ${GLOG_ROOT_DIR}
        PATH_SUFFIXES Debug)

    set(GLOG_LIBRARY optimized ${GLOG_LIBRARY_RELEASE} debug ${GLOG_LIBRARY_DEBUG})
else()
    find_library(GLOG_LIBRARY NAMES glog libglog.a HINTS ${GLOG_ROOT_DIR}/lib)
endif()

find_package_handle_standard_args(GLOG DEFAULT_MSG
    GLOG_INCLUDE_DIR GLOG_LIBRARY)

if(GLOG_FOUND)
    set(GLOG_INCLUDE_DIRS ${GLOG_INCLUDE_DIR})
    set(GLOG_LIBRARIES ${GLOG_LIBRARY})
endif()

MESSAGE(STATUS "GLOG_INCLUDE_DIR --> " ${GLOG_INCLUDE_DIR})
MESSAGE(STATUS "GLOG_LIBRARY --> " ${GLOG_LIBRARY})