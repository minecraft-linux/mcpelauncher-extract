# Finds the `libzip` library.
# This file is released under the Public Domain.
# Once done this will define
#  LIBZIP_FOUND - Set to true if libzip has been found
#  LIBZIP_INCLUDE_DIRS - The libzip include directories
#  LIBZIP_LIBRARIES - The libraries needed to use libzip

find_package(PkgConfig)
pkg_check_modules(PC_LIBZIP QUIET libzip)

find_path(LIBZIP_INCLUDE_DIR
        NAMES zip.h
        PATH_SUFFIXES libzip
        HINTS ${PC_LIBZIP_INCLUDEDIR} ${PC_LIBZIP_INCLUDE_DIRS})
find_library(LIBZIP_LIBRARY
        NAMES zip libzip
        HINTS ${PC_LIBZIP_LIBDIR} ${PC_LIBZIP_LIBRARY_DIRS})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LIBZIP DEFAULT_MSG
        LIBZIP_LIBRARY LIBZIP_INCLUDE_DIR)

mark_as_advanced(LIBZIP_INCLUDE_DIR LIBZIP_LIBRARY)

set(LIBZIP_LIBRARIES ${LIBZIP_LIBRARY})
set(LIBZIP_INCLUDE_DIRS ${LIBZIP_INCLUDE_DIR})