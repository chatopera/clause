# - Try to find Baidu LAC
#
# The following variables are optionally searched for defaults
#  PADDLE_ROOT_DIR:            Base directory where all LAC components are found
#
# The following are set after configuration is done:
#  PADDLE_ABS_PATH

include(FindPackageHandleStandardArgs)

set(PADDLE_ROOT_DIR ${PROJECT_SOURCE_DIR}/lib/paddle CACHE PATH "Folder contains Baidu LAC")

if (IS_ABSOLUTE ${PADDLE_ROOT_DIR})
    set(PADDLE_ABS_PATH ${PADDLE_ROOT_DIR})
else ()
    get_filename_component(PADDLE_ABS_PATH ${CMAKE_BINARY_DIR}/${PADDLE_ROOT} ABSOLUTE)
endif ()

if (EXISTS ${PADDLE_ABS_PATH})
    message(STATUS "\$PADDLE_ABS_PATH is set to '${PADDLE_ABS_PATH}'")
else ()
    message(FATAL_ERROR "'${PADDLE_ABS_PATH}' does not exist.\nPlease set -DPADDLE_ROOT=/path/to/your/fluid_inference_lib/")
endif ()