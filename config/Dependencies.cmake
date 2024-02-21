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

# Application Dependencies #
############################

# Default build Qt version is 6
if (NOT DEFINED QT_VERSION)
    set(QT_VERSION 6)
endif()

message(STATUS "Compile with Qt version: ${QT_VERSION}")

find_package(QT NAMES Qt6 COMPONENTS Core Gui BuildInternals REQUIRED)
find_package(Qt6 COMPONENTS Quick QuickTest REQUIRED)
find_package(Qt6 COMPONENTS Test REQUIRED)
find_package(Qt6 COMPONENTS ShaderTools REQUIRED)
find_package(Qt6 COMPONENTS OpenGL REQUIRED)

find_package(LIBVLC 4.0.0 REQUIRED)

SET(QT_INSTALL_QML ${CMAKE_INSTALL_PREFIX}/qml)

# Add libVLC libraries
# Define version, if not defined use latest relased
SET(LIBVLC_VERSION 0x040000 CACHE STRING "libVLC version used")
