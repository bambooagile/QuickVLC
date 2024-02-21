#############################################################################
# VLC - CMake module
# Copyright (C) 2014 Tadej Novak <tadej@tano.si>
# Original author: Rohit Yadav <rohityadav89@gmail.com>
#
# This library is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published
# by the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this library. If not, see <http://www.gnu.org/licenses/>.
#############################################################################
# If it's found it sets LIBVLC_FOUND to TRUE
# and following variables are set:
#  LIBVLC_INCLUDE_DIR
#  LIBVLC_LIBRARY


FIND_PACKAGE(PkgConfig)
PKG_CHECK_MODULES(PC_LIBVLC QUIET "libvlc")

# FIND_PATH and FIND_LIBRARY normally search standard locations
# before the specified paths. To search non-standard paths first,
# FIND_* is invoked first with specified paths and NO_DEFAULT_PATH
# and then again with no specified paths to search the default
# locations. When an earlier FIND_* succeeds, subsequent FIND_*s
# searching for the same item do nothing.

#Put here path to custom location
#example: /home/user/vlc/include etc..
FIND_PATH(LIBVLC_INCLUDE_DIR vlc/vlc.h
  NO_DEFAULT_PATH
  HINTS "$ENV{LIBVLC_INCLUDE_PATH}"
  PATHS
    ${PC_LIBVLC_INCLUDE_DIRS}
    #Mac OS and Contribs
    "${CMAKE_CURRENT_SOURCE_DIR}/contribs/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/contribs/include/vlc"
    "/Applications/VLC.app/Contents/MacOS/include"
    "/Applications/VLC.app/Contents/MacOS/include/vlc"
    # Env
    "$ENV{LIB_DIR}/include"
    "$ENV{LIB_DIR}/include/vlc"
    #
    "/usr/include"
    "/usr/include/vlc"
    "/usr/local/include"
    "/usr/local/include/vlc"
    #mingw
    c:/msys/local/include
    "c:/Program Files (x86)/VideoLAN/VLC/sdk/include"
  )
FIND_PATH(LIBVLC_INCLUDE_DIR PATHS "${CMAKE_INCLUDE_PATH}/vlc" NAMES vlc.h)

#Put here path to custom location
#example: /home/user/vlc/lib etc..
FIND_LIBRARY(LIBVLC_LIBRARY NAMES vlc libvlc
  NO_DEFAULT_PATH
  HINTS "$ENV{LIBVLC_LIBRARY_PATH}"
  PATHS
    ${PC_LIBVLC_LIBRARY_DIRS}
    "$ENV{LIB_DIR}/lib"
    #Mac OS
    "${CMAKE_CURRENT_SOURCE_DIR}/contribs/lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/contribs/plugins"
    "/Applications/VLC.app/Contents/MacOS/lib"
    "/Applications/VLC.app/Contents/MacOS/plugins"
    #mingw
    c:/msys/local/lib
    "c:/Program Files (x86)/VideoLAN/VLC/sdk/lib"
  )
FIND_LIBRARY(LIBVLC_LIBRARY NAMES vlc libvlc)
FIND_LIBRARY(LIBVLCCORE_LIBRARY NAMES vlccore libvlccore
  NO_DEFAULT_PATH
  HINTS "$ENV{LIBVLC_LIBRARY_PATH}"
  PATHS
    ${PC_LIBVLC_LIBRARY_DIRS}
    "$ENV{LIB_DIR}/lib"
    #Mac OS
    "${CMAKE_CURRENT_SOURCE_DIR}/contribs/lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/contribs/plugins"
    "/Applications/VLC.app/Contents/MacOS/lib"
    "/Applications/VLC.app/Contents/MacOS/plugins"
    #mingw
    c:/msys/local/lib
    "c:/Program Files (x86)/VideoLAN/VLC/sdk/lib"
  )
FIND_LIBRARY(LIBVLCCORE_LIBRARY NAMES vlccore libvlccore)

## extract VLC version
IF(LIBVLC_INCLUDE_DIR AND EXISTS "${LIBVLC_INCLUDE_DIR}/vlc/libvlc_version.h")
  FILE(STRINGS "${LIBVLC_INCLUDE_DIR}/vlc/libvlc_version.h" LIBVLC_VERSION_MAJOR_LINE REGEX "^# define[ \t]+LIBVLC_VERSION_MAJOR[ \t]+\\([0-9]+\\)$")
  FILE(STRINGS "${LIBVLC_INCLUDE_DIR}/vlc/libvlc_version.h" LIBVLC_VERSION_MINOR_LINE REGEX "^# *define[ \t]+LIBVLC_VERSION_MINOR[ \t]+\\([0-9]+\\)$")
  FILE(STRINGS "${LIBVLC_INCLUDE_DIR}/vlc/libvlc_version.h" LIBVLC_VERSION_PATCH_LINE REGEX "^# *define[ \t]+LIBVLC_VERSION_REVISION[ \t]+\\([0-9]+\\)$")
  STRING(REGEX REPLACE "^# *define[ \t]+LIBVLC_VERSION_MAJOR[ \t]+\\(([0-9]+)\\)$" "\\1"    LIBVLC_VERSION_MAJOR "${LIBVLC_VERSION_MAJOR_LINE}")
  STRING(REGEX REPLACE "^# *define[ \t]+LIBVLC_VERSION_MINOR[ \t]+\\(([0-9]+)\\)$" "\\1"    LIBVLC_VERSION_MINOR "${LIBVLC_VERSION_MINOR_LINE}")
  STRING(REGEX REPLACE "^# *define[ \t]+LIBVLC_VERSION_REVISION[ \t]+\\(([0-9]+)\\)$" "\\1" LIBVLC_VERSION_PATCH "${LIBVLC_VERSION_PATCH_LINE}")
  SET(LIBVLC_VERSION_STRING ${LIBVLC_VERSION_MAJOR}.${LIBVLC_VERSION_MINOR}.${LIBVLC_VERSION_PATCH})
  UNSET(LIBVLC_VERSION_MAJOR_LINE)
  UNSET(LIBVLC_VERSION_MINOR_LINE)
  UNSET(LIBVLC_VERSION_PATCH_LINE)
  UNSET(LIBVLC_VERSION_MAJOR)
  UNSET(LIBVLC_VERSION_MINOR)
  UNSET(LIBVLC_VERSION_PATCH)
ENDIF()

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LIBVLC
  FOUND_VAR LIBVLC_FOUND
  REQUIRED_VARS
    LIBVLC_LIBRARY
    LIBVLC_INCLUDE_DIR
  VERSION_VAR LIBVLC_VERSION_STRING
)

IF (LIBVLC_FOUND)
	SET(LIBVLC_LIBRARIES ${LIBVLC_LIBRARY})
	SET(LIBVLC_INCLUDE_DIRS ${LIBVLC_INCLUDE_DIR})
	SET(LIBVLC_DEFINITIONS ${PC_LIBVLC_CFLAGS_OTHER})
ELSE (LIBVLC_FOUND)
   IF (LIBVLC_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "Could not find LibVLC")
   ENDIF (LIBVLC_FIND_REQUIRED)
ENDIF (LIBVLC_FOUND)

IF(LIBVLC_FOUND AND NOT TARGET LIBVLC::LIBVLC)
  ADD_LIBRARY(LIBVLC::LIBVLC UNKNOWN IMPORTED)
  SET_TARGET_PROPERTIES(LIBVLC::LIBVLC PROPERTIES
    IMPORTED_LOCATION "${LIBVLC_LIBRARY}"
    INTERFACE_COMPILE_OPTIONS "${PC_LIBVLC_CFLAGS_OTHER}"
    INTERFACE_INCLUDE_DIRECTORIES "${LIBVLC_INCLUDE_DIR}"
  )
ENDIF()

MARK_AS_ADVANCED(
  LIBVLC_INCLUDE_DIR
  LIBVLC_LIBRARY
)
