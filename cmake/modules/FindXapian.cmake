# - Try to find Xapian.
# Once done this will define:
# XAPIAN_FOUND			- If false, do not try to use xapian.
# XAPIAN_INCLUDE_DIR	- Where to find xapian.h, etc.
# XAPIAN_LIBRARIES		- The libraries to link against.
#
# Created by Hai Liang Wang.
#
# Please note that this module only supports Linux officially, but
# should work on all UNIX-like operational systems too.
#

#=============================================================================
# Copyright 2019 Chatopera Inc.
#
# All rights resvered.
#=============================================================================
# (To distribute this file outside of CMake, substitute the full
#  License text for the above reference.)

set(XAPIAN_ROOT_DIR ${PROJECT_SOURCE_DIR} CACHE PATH "Folder contains Xapian Libs")

find_path( XAPIAN_INCLUDE_DIR
    NAMES "xapian.h"
    PATHS "${XAPIAN_ROOT_DIR}/include")

find_library( XAPIAN_LIBRARY
    NAMES "xapian"
    PATHS "${XAPIAN_ROOT_DIR}/lib" )

include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( XAPIAN DEFAULT_MSG
	XAPIAN_LIBRARY XAPIAN_INCLUDE_DIR)

set( XAPIAN_INCLUDE_DIRS ${XAPIAN_INCLUDE_DIR} )
set( XAPIAN_LIBRARIES ${XAPIAN_LIBRARY} )

mark_as_advanced( XAPIAN_INCLUDE_DIR XAPIAN_LIBRARY )

#MESSAGE(STATUS "XAPIAN_FOUND --> " ${XAPIAN_FOUND})
MESSAGE(STATUS "XAPIAN_INCLUDE_DIR --> " ${XAPIAN_INCLUDE_DIR})
MESSAGE(STATUS "XAPIAN_LIBRARY --> " ${XAPIAN_LIBRARY})