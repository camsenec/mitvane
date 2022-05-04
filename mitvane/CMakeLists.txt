#  This file is modified
#  by Tomoya Tanaka <deepsky2221@gmail.com>
#  from <https://github.com/riebl/vanetza/blob/master/tools/socktap/CMakeLists.txt>
#  at 2022-02-25.
#  
#  This file is part of Mitvane.
# 
#  Mitvane is free software: you can redistribute it and/or modify it 
#  under the terms of the GNU Lesser General Public License as published
#  by the Free Software Foundation, either version 3 of the License, or 
#  any later version.
#  Mitvane is distributed in the hope that it will be useful, 
#  but WITHOUT ANY WARRANTY; without even the implied warranty of 
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
#  See the GNU General Public License and 
#  GNU Lesser General Public License for more details.
#  
#  You should have received a copy of the GNU General Public License 
#  and GNU Lesser General Public License along with Mitvane. 
#  If not, see <https://www.gnu.org/licenses/>. 

#  State changes
#  - The target name is changed.
#  - Some source files are added/removed to generate executable target mitvane.
#  - Dependent library Vanetza-extension::vanetza-extension is added.
#  - The name of the target to be installed is changed to mitvane.


if(NOT TARGET Boost::system)
    message(STATUS "Skip build of mitvane because of missing Boost::system dependency")
    return()
endif()

if(NOT TARGET Boost::program_options)
    message(STATUS "Skip build of mitvane because of missing Boost::program_options dependency")
    return()
endif()

find_package(Threads REQUIRED)

add_subdirectory(decoder)
add_subdirectory(classifier)
add_subdirectory(app_layer_parser)
add_subdirectory(rule_reader)
add_subdirectory(detector)
add_subdirectory(message_handler)

add_executable(mitvane
    ethernet_device.cpp
    link_layer.cpp
    main.cpp
    positioning.cpp
    raw_socket_link.cpp
    idps_context.cpp
)

target_link_libraries(mitvane
    PUBLIC Boost::system
    Boost::program_options
    Boost::log_setup
    Boost::log
    Threads::Threads 
    Vanetza-extension::vanetza-extension
    Vanetza::btp
    Vanetza::geonet
    Mitvane::decoder
    Mitvane::classifier
    Mitvane::app_layer_parser
    Mitvane::rule_reader
    Mitvane::detector
    Mitvane::message_handler)

install(TARGETS mitvane EXPORT ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR})

option(mitvane_WITH_COHDA_LLC "Use Cohda LLC API for mitvane" ${COHDA_FOUND})
if(mitvane_WITH_COHDA_LLC)
    find_package(Cohda MODULE REQUIRED)
    target_compile_definitions(mitvane PUBLIC "mitvane_WITH_COHDA_LLC")
    target_include_directories(mitvane PUBLIC ${COHDA_INCLUDE_DIRS})
    target_sources(mitvane PRIVATE cohda.cpp cohda_link.cpp)
endif()

find_package(GPS QUIET)
option(MITVANE_WITH_GPSD "Enable gpsd positioning for mitvane" ${GPS_FOUND})
if(MITVANE_WITH_GPSD)
    find_package(GPS REQUIRED)
    target_compile_definitions(mitvane PUBLIC "mitvane_WITH_GPSD")
    target_link_libraries(mitvane PUBLIC GPS::GPS)
    target_sources(mitvane PRIVATE gps_position_provider.cpp)
endif()
