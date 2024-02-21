###############################################################################
# This file is part of QuickVLC - Qt and libvlc connection library.
# Copyright (C) 2016 Tadej Novak <tadej@tano.si>, 
#               2022 Alexey Avramchik (OU Bamboo group) <aa@bam-boo.eu>
#
# QuickVLC is free software: you can redistribute it and/or modify it 
# under the terms of the GNU General Public License as published 
# by the Free Software Foundation, either version 3 of the License, 
# or (at your option) any later version.
#
# QuickVLC is distributed in the hope that it will be useful, 
# but WITHOUT ANY WARRANTY; without even the implied warranty 
# of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
# See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License 
# along with QuickVLC. If not, see <https://www.gnu.org/licenses/>.
###############################################################################

message(STATUS "Configuring Core library")

set(QUICKVLC_CORE_HEADERS
    core_shared_export.h
    audio.h
    error.h
    common.h
    instance.h
    mediaplayer.h
    media.h
    abstractvideostream.h
    vlc.h
    abstractvideoframe.h
    videoframepool.h
)

set(QUICKVLC_CORE_SOURCES
    audio.cpp
    error.cpp
    common.cpp
    instance.cpp
    mediaplayer.cpp
    media.cpp
    abstractvideostream.cpp
    vlc.cpp
    abstractvideoframe.cpp
    videoframepool.cpp
)

IF(MSVC OR MINGW)
    list(APPEND QUICKVLC_CORE_SOURCES
        compat/asprintf.c
        compat/vasprintf.c
    )
    list(APPEND QUICKVLC_CORE_SOURCES
        d3d11videostream.cpp
    )
    list(APPEND QUICKVLC_CORE_HEADERS
        d3d11videostream.h
    )
ENDIF()
list(APPEND QUICKVLC_CORE_HEADERS
    openglvideostream.h
    openglvideoframe.h
)
list(APPEND QUICKVLC_CORE_SOURCES
    openglvideostream.cpp
    openglvideoframe.cpp
)


add_library(${QUICKVLC_CORE} SHARED ${QUICKVLC_CORE_SOURCES} ${QUICKVLC_CORE_HEADERS} )

set_target_properties(${QUICKVLC_CORE} PROPERTIES
    FRAMEWORK ON
    OUTPUT_NAME "${QUICKVLC_CORE_NAME}"
    VERSION ${QUICKVLC_VERSION}
    SOVERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}"
    PUBLIC_HEADER "${QUICKVLC_CORE_HEADERS}"
)

target_include_directories(${QUICKVLC_CORE}
    PRIVATE
        ${CMAKE_BINARY_DIR}/src
        ${LIBVLC_EXTRA_INCLUDE_DIR}
        ${LIBVLC_EXTRA_INCLUDE_DIR}/vlc/plugins
        ${LIBVLC_INCLUDE_DIR}
        ${LIBVLC_INCLUDE_DIR}/vlc/plugins
)

target_link_libraries(${QUICKVLC_CORE}
    PRIVATE
        Qt6::Core
        Qt6::Quick
        Qt6::OpenGL
        LIBVLC::LIBVLC
)

target_compile_definitions(${QUICKVLC_CORE} PRIVATE QUICKVLC_CORE_LIBRARY)

# Additional macros
generate_pkgconfig(${QUICKVLC_CORE_NAME} core)
generate_windows_rc(${QUICKVLC_CORE_NAME} core QUICKVLC_CORE_SOURCES)

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(QuickVLCCoreIncludesLocation ${CMAKE_INSTALL_LIBDIR}/${QUICKVLC_CORE_NAME}.framework)
    set(QuickVLCCorePublicHeaderLocation ${CMAKE_INSTALL_LIBDIR}/${QUICKVLC_CORE_NAME}.framework)
else()
    set(QuickVLCCoreIncludesLocation ${CMAKE_INSTALL_INCLUDEDIR})
    set(QuickVLCCorePublicHeaderLocation ${CMAKE_INSTALL_INCLUDEDIR}/${QUICKVLC_CORE_NAME})
endif()


install(TARGETS ${QUICKVLC_CORE} EXPORT QuickVLCCoreTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    FRAMEWORK DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    INCLUDES DESTINATION ${QuickVLCCoreIncludesLocation}
    PUBLIC_HEADER DESTINATION ${QuickVLCCorePublicHeaderLocation}
)
