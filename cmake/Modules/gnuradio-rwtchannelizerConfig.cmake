find_package(PkgConfig)

PKG_CHECK_MODULES(PC_GR_RWTCHANNELIZER gnuradio-rwtchannelizer)

FIND_PATH(
    GR_RWTCHANNELIZER_INCLUDE_DIRS
    NAMES gnuradio/rwtchannelizer/api.h
    HINTS $ENV{RWTCHANNELIZER_DIR}/include
        ${PC_RWTCHANNELIZER_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    GR_RWTCHANNELIZER_LIBRARIES
    NAMES gnuradio-rwtchannelizer
    HINTS $ENV{RWTCHANNELIZER_DIR}/lib
        ${PC_RWTCHANNELIZER_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
          )

include("${CMAKE_CURRENT_LIST_DIR}/gnuradio-rwtchannelizerTarget.cmake")

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GR_RWTCHANNELIZER DEFAULT_MSG GR_RWTCHANNELIZER_LIBRARIES GR_RWTCHANNELIZER_INCLUDE_DIRS)
MARK_AS_ADVANCED(GR_RWTCHANNELIZER_LIBRARIES GR_RWTCHANNELIZER_INCLUDE_DIRS)
