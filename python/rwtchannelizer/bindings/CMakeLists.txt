# Copyright 2020 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Check if there is C++ code at all
########################################################################
if(NOT rwtchannelizer_sources)
    MESSAGE(STATUS "No C++ sources... skipping python bindings")
    return()
endif(NOT rwtchannelizer_sources)

########################################################################
# Check for pygccxml
########################################################################
GR_PYTHON_CHECK_MODULE_RAW(
    "pygccxml"
    "import pygccxml"
    PYGCCXML_FOUND
    )

include(GrPybind)

########################################################################
# Python Bindings
########################################################################

list(APPEND rwtchannelizer_python_files
    rwt_source_chan_python.cc python_bindings.cc)

GR_PYBIND_MAKE_OOT(rwtchannelizer
   ../../..
   gr::rwtchannelizer
   "${rwtchannelizer_python_files}")

# copy bindings extension for use in QA test module
add_custom_command(TARGET rwtchannelizer_python POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:rwtchannelizer_python>
    ${PROJECT_BINARY_DIR}/test_modules/gnuradio/rwtchannelizer/
)

install(TARGETS rwtchannelizer_python DESTINATION ${GR_PYTHON_DIR}/gnuradio/rwtchannelizer COMPONENT pythonapi)
