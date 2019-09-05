# - Try to find ActiveMQcpp
# Once done this will define
#
#  ACTIVEMQCPP_FOUND - System has ActiveMQcpp
#  ACTIVEMQCPP_INCLUDE_DIR - The ActiveMQcpp include directory
#  ACTIVEMQCPP_LIBRARIES - The libraries needed to use ActiveMQcpp
#  ACTIVEMQCPP_DEFINITIONS - Compiler switches required for using ActiveMQcpp
#  ACTIVEMQCPP_LINKDEFINITIONS - Linker switches needed to link with ActiveMQcpp
#
#
# Setting the var
#  ACTIVEMQCPP_ALTLOCATION
#  will enforce the search for the libraries/executables in a different place
#  with respect to the eventual system-wide setup

#=============================================================================
# By Fabrizio Furano (CERN IT/GT), Oct 2010


IF (ACTIVEMQCPP_INCLUDE_DIR AND ACTIVEMQCPP_LIBRARIES)
  # in cache already
  SET(ACTIVEMQCPP_FIND_QUIETLY TRUE)
ENDIF (ACTIVEMQCPP_INCLUDE_DIR AND ACTIVEMQCPP_LIBRARIES)

IF (NOT WIN32)
  # use pkg-config to get the directories and then use these values
  # in the FIND_PATH() and FIND_LIBRARY() calls
  if (NOT LIBDESTINATION)
    set(LIBDESTINATION "lib")
  endif()


  FIND_PACKAGE(PkgConfig)
  IF (ACTIVEMQCPP_ALTLOCATION)
    set( ENV{PKG_CONFIG_PATH} "${ACTIVEMQCPP_ALTLOCATION}/${LIBDESTINATION}/pkgconfig:${ACTIVEMQCPP_ALTLOCATION}/lib/pkgconfig:$ENV{PKG_CONFIG_PATH}" )
    MESSAGE(STATUS "Looking for ActiveMQcpp package in $ENV{PKG_CONFIG_PATH}")
  ENDIF()
  PKG_CHECK_MODULES(PC_ACTIVEMQCPP activemq-cpp)

  foreach(i ${PC_ACTIVEMQCPP_CFLAGS})
#    message(STATUS "${i}")
    SET(ACTIVEMQCPP_DEFINITIONS "${ACTIVEMQCPP_DEFINITIONS} ${i}")
  endforeach()

  foreach(i ${PC_ACTIVEMQCPP_LDFLAGS})
#    message(STATUS "${i}")
    SET(ACTIVEMQCPP_LINKDEFINITIONS "${ACTIVEMQCPP_LINKDEFINITIONS} ${i}")
  endforeach()
  SET(ACTIVEMQCPP_VERSION ${PC_ACTIVEMQCPP_VERSION})
ENDIF (NOT WIN32)

IF (NOT ACTIVEMQCPP_ALTLOCATION)
  SET(ACTIVEMQCPP_ALTLOCATION "" CACHE PATH "An alternate forced location for atcivemq-cpp")
ENDIF()


# Find the includes, by giving priority to ACTIVEMQCPP_ALTLOCATION, if set
IF (ACTIVEMQCPP_ALTLOCATION)
  find_path(ACTIVEMQCPP_INCLUDE_DIR activemq/library/ActiveMQCPP.h DOC "Path to the activemq-cpp include file"
    HINTS
    ${ACTIVEMQCPP_ALTLOCATION}/include
    ${ACTIVEMQCPP_ALTLOCATION}/include/*
    NO_SYSTEM_ENVIRONMENT_PATH
    )
  IF(ACTIVEMQCPP_INCLUDE_DIR)
    MESSAGE(STATUS "activemq-cpp includes found in ${ACTIVEMQCPP_INCLUDE_DIR}")
  else()
    MESSAGE(FATAL_ERROR "activemq-cpp includes not found. You may want to suggest a location by using -DACTIVEMQCPP_ALTLOCATION=<location of AMQCPP>")
  ENDIF()

ENDIF (ACTIVEMQCPP_ALTLOCATION)

# Now look in the system wide places, if not yet found
find_path(ACTIVEMQCPP_INCLUDE_DIR activemq/library/ActiveMQCPP.h DOC "Path to the activemq-cpp include file"
  HINTS
  ${PC_ACTIVEMQCPP_INCLUDEDIR}
  ${PC_ACTIVEMQCPP_INCLUDE_DIRS}
  ${PC_ACTIVEMQCPP_INCLUDEDIR}/*
  )
IF(ACTIVEMQCPP_INCLUDE_DIR)
  MESSAGE(STATUS "activemq-cpp includes found in ${ACTIVEMQCPP_INCLUDE_DIR}")
else()
  MESSAGE(FATAL_ERROR "activemq-cpp includes not found. You may want to suggest a location by using -DACTIVEMQCPP_ALTLOCATION=<location of AMQCPP>")
ENDIF()

# Find the libs, by giving priority to ACTIVEMQCPP_ALTLOCATION, if set
IF (ACTIVEMQCPP_ALTLOCATION)
  find_library(ACTIVEMQCPP_LIBRARIES NAMES libactivemq-cpp.a activemq-cpp DOC "Path to the activemq-cpp library"
    HINTS
    ${ACTIVEMQCPP_ALTLOCATION}
    ${ACTIVEMQCPP_ALTLOCATION}/lib
    NO_SYSTEM_ENVIRONMENT_PATH
    )
  IF(ACTIVEMQCPP_LIBRARIES)
    MESSAGE(STATUS "activemq-cpp lib found in ${ACTIVEMQCPP_LIBRARIES}")
  else()
    MESSAGE(FATAL_ERROR "activemq-cpp lib not found. You may want to suggest a location by using -DACTIVEMQCPP_ALTLOCATION=<location of AMQCPP>")
  ENDIF()
ENDIF (ACTIVEMQCPP_ALTLOCATION)

# Now look in the system wide places, if not yet found
FIND_LIBRARY(ACTIVEMQCPP_LIBRARIES NAMES libactivemq-cpp.a activemq-cpp DOC "Path to the activemq-cpp library"
  HINTS
  ${PC_ACTIVEMQCPP_LIBDIR}
  ${PC_ACTIVEMQCPP_LIBRARY_DIRS}
  )
IF(ACTIVEMQCPP_LIBRARIES)
  MESSAGE(STATUS "activemq-cpp lib found in ${ACTIVEMQCPP_LIBRARIES}")
else()
  MESSAGE(FATAL_ERROR "activemq-cpp lib not found. You may want to suggest a location by using -DACTIVEMQCPP_ALTLOCATION=<location of AMQCPP>")
ENDIF()






#
# Version checking
#
if(ActiveMQcpp_FIND_VERSION AND ACTIVEMQCPP_VERSION)
    if(ActiveMQcpp_FIND_VERSION_EXACT)
        if(NOT ACTIVEMQCPP_VERSION VERSION_EQUAL ${ActiveMQcpp_FIND_VERSION})
	  MESSAGE(FATAL_ERROR "Found ACTIVEMQCPP ${ACTIVEMQCPP_VERSION} != ${ActiveMQcpp_FIND_VERSION}")
        endif()
    else()
        # version is too low
        if(NOT ACTIVEMQCPP_VERSION VERSION_EQUAL ${ActiveMQcpp_FIND_VERSION} AND 
                NOT ACTIVEMQCPP_VERSION VERSION_GREATER ${ActiveMQcpp_FIND_VERSION})
	      MESSAGE(FATAL_ERROR "Found ACTIVEMQCPP ${ACTIVEMQCPP_VERSION} < ${ActiveMQcpp_FIND_VERSION} (Minimum required). You may want to suggest a location by using -DACTIVEMQCPP_ALTLOCATION=<location of AMQCPP>")
        endif()
    endif()
endif()

INCLUDE(FindPackageHandleStandardArgs)

# handle the QUIETLY and REQUIRED arguments and set ACTIVEMQCPP_FOUND to TRUE if 
# all listed variables are TRUE
FIND_PACKAGE_HANDLE_STANDARD_ARGS(ACTIVEMQCPP DEFAULT_MSG ACTIVEMQCPP_LIBRARIES ACTIVEMQCPP_INCLUDE_DIR)

MARK_AS_ADVANCED(ACTIVEMQCPP_INCLUDE_DIR ACTIVEMQCPP_LIBRARIES)

MESSAGE(STATUS "ACTIVEMQCPP_LIBRARIES --> " ${ACTIVEMQCPP_LIBRARIES})
MESSAGE(STATUS "ACTIVEMQCPP_INCLUDE_DIR --> " ${ACTIVEMQCPP_INCLUDE_DIR})
