# - Try to find Threads
# Once done this will define
# Threads_FOUND - System has Threads
# THREADS_INCLUDE_DIR - The Threads include directories
# THREADS_LIBRARY - The libraries needed to use bcm2835

find_path ( Threads_INCLUDE_DIR pthread.h )
find_library ( Threads_LIBRARY NAMES libpthread.so libpthread.a)

set ( THREADS_LIBRARY ${Threads_LIBRARY} )
set ( THREADS_INCLUDE_DIR ${Threads_INCLUDE_DIR} )

include ( FindPackageHandleStandardArgs )
MESSAGE(STATUS "THREADS_LIBRARY --> " ${THREADS_LIBRARY})
# handle the QUIETLY and REQUIRED arguments and set Threads_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args ( Threads DEFAULT_MSG Threads_LIBRARY Threads_INCLUDE_DIR )