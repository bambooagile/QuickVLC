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

message(STATUS "Configuring QML Components library")

SET(QUICKVLC_QML_HEADERS
    qml_shared_export.h
    audiooutput.h
    videooutput.h
    mediasource.h
    videooutput.h
    mediaplayer.h
    rendering/videomaterial.h
    rendering/videonode.h
    rendering/videomaterialshader.h
    rendering/videotexture.h
    video/videostream.h
)

SET(QUICKVLC_QML_SOURCES
    audiooutput.cpp
    videooutput.cpp
    mediasource.cpp
    videooutput.cpp
    mediaplayer.cpp
    rendering/videomaterial.cpp
    rendering/videonode.cpp
    rendering/videomaterialshader.cpp
    rendering/videotexture.cpp
    video/videostream.cpp
)

qt_add_qml_module(${QUICKVLC_QML}
    SHARED
    URI QuickVLC
    VERSION 1.0
    SOURCES
        ${QUICKVLC_QML_HEADERS} ${QUICKVLC_QML_SOURCES}
)

add_dependencies(${QUICKVLC_QML} ${QUICKVLC_CORE})

qt_add_shaders(${QUICKVLC_QML} "shaders"
    BATCHABLE
    PREFIX "/shaders"
    FILES
        rendering/shaders/videomaterialshader.vert
        rendering/shaders/videomaterialshader.frag
)

target_include_directories(${QUICKVLC_QML}
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/src/qml
        ${CMAKE_BINARY_DIR}/src
        ${CMAKE_BINARY_DIR}/include
)

target_link_libraries(${QUICKVLC_QML}
    PRIVATE
        Qt6::Core
        Qt6::Quick
        Qt6::QuickPrivate
        LIBVLC::LIBVLC
        ${QUICKVLC_CORE}
)

add_custom_target(${QUICKVLC_QML}_shaders
    COMMENT "Just shaders files"
    SOURCES
        rendering/shaders/videomaterialshader.vert
        rendering/shaders/videomaterialshader.frag
)

set_target_properties(${QUICKVLC_QML} PROPERTIES
    FRAMEWORK ON
    OUTPUT_NAME "${QUICKVLC_QML_NAME}"
    VERSION ${QUICKVLC_VERSION}
    SOVERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}"
    PUBLIC_HEADER "${QUICKVLC_QML_HEADERS}"
)

set_target_properties(${QUICKVLC_QML}plugin PROPERTIES
    FRAMEWORK OFF
)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    SET_TARGET_PROPERTIES(${QUICKVLC_QML}plugin PROPERTIES INSTALL_RPATH "@loader_path/../../lib")
ENDIF()

target_compile_definitions(${QUICKVLC_QML} PRIVATE QUICKVLC_QML_LIBRARY)

# Additional macros
generate_pkgconfig(${QUICKVLC_QML_NAME} qml)
generate_windows_rc(${QUICKVLC_QML_NAME} qml QUICKVLC_SOURCES)

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(QuickVLCQmlIncludesLocation ${CMAKE_INSTALL_LIBDIR}/${QUICKVLC_QML_NAME}.framework)
    set(QuickVLCQmlPublicHeaderLocation ${CMAKE_INSTALL_LIBDIR}/${QUICKVLC_QML_NAME}.framework)
else()
    set(QuickVLCQmlIncludesLocation ${CMAKE_INSTALL_INCLUDEDIR})
    set(QuickVLCQmlPublicHeaderLocation ${CMAKE_INSTALL_INCLUDEDIR}/${QUICKVLC_QML_NAME})
endif()


install(TARGETS ${QUICKVLC_QML} EXPORT QuickVLCQmlTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    FRAMEWORK DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    INCLUDES DESTINATION ${QuickVLCQmlIncludesLocation}
    PUBLIC_HEADER DESTINATION ${QuickVLCQmlPublicHeaderLocation}
)

install(TARGETS ${QUICKVLC_QML}plugin
    RUNTIME DESTINATION ${QT_INSTALL_QML}/${QUICKVLC_PLUGIN_QML_NAME}
    LIBRARY DESTINATION ${QT_INSTALL_QML}/${QUICKVLC_PLUGIN_QML_NAME}
    ARCHIVE DESTINATION ${QT_INSTALL_QML}/${QUICKVLC_PLUGIN_QML_NAME}
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/qmldir ${CMAKE_CURRENT_BINARY_DIR}/Qml.qmltypes
    DESTINATION ${QT_INSTALL_QML}/${QUICKVLC_PLUGIN_QML_NAME}
)
