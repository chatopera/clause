# - Try to find CRFsuite
#
# The following variables are optionally searched for defaults
#  CRFSUITE_ROOT_DIR:            Base directory where all CRFsuite components are found
#
# The following are set after configuration is done:
#  CRFSUITE_FOUND
#  CRFSUITE_INCLUDE_DIR
#  CRFSUITE_LIBRARY

include(FindPackageHandleStandardArgs)

set(CRFSUITE_ROOT_DIR ${PROJECT_SOURCE_DIR} CACHE PATH "Folder contains CRFsuite")

find_path(CRFSUITE_INCLUDE_DIR crfsuite.h
        PATHS ${CRFSUITE_ROOT_DIR}/include)

find_library(CRFSUITE_LIBRARY 
        NAMES libcrfsuite.a crfsuite 
        HINTS ${CRFSUITE_ROOT_DIR}/lib)

if(EXISTS "${CRFSUITE_ROOT_DIR}/lib/libcqdb-0.12.so")
    set(CRFSUITE_LIBRARY ${CRFSUITE_LIBRARY}
                        "${CRFSUITE_ROOT_DIR}/lib/libcqdb-0.12.so")
                        
else(EXISTS "${CRFSUITE_ROOT_DIR}/lib/libcqdb-0.12.so")
    set(CRFSUITE_LIBRARY FALSE)
endif()

if(CRFSUITE_INCLUDE_DIR AND CRFSUITE_LIBRARY)
    set(CRFSUITE_FOUND TRUE)
	mark_as_advanced(CRFSUITE_INCLUDE_DIR CRFSUITE_LIBRARY)
endif(CRFSUITE_INCLUDE_DIR AND CRFSUITE_LIBRARY)

find_package_handle_standard_args(CRFSUITE DEFAULT_MSG
	CRFSUITE_LIBRARY CRFSUITE_INCLUDE_DIR)

MESSAGE(STATUS "CRFSUITE_LIBRARY --> " ${CRFSUITE_LIBRARY})
MESSAGE(STATUS "CRFSUITE_INCLUDE_DIR --> " ${CRFSUITE_INCLUDE_DIR})
