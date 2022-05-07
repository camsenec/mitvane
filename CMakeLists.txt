#  (C) 2022 Tomoya Tanaka <deepsky2221@gmail.com> 
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


cmake_minimum_required(VERSION 3.1)
project(Mitvane VERSION 0.1)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# enable convenient CMake policies if available
if(POLICY CMP0074)
  # find_package() employs <package>_ROOT (since CMake 3.12)
  cmake_policy(SET CMP0074 NEW)
endif()
if(POLICY CMP0077)
  # option() honours normal variable (since CMake 3.13)
  cmake_policy(SET CMP0077 NEW)
endif()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-g -Wall -Wno-sign-compare)

# Look up threading library (usually pthread)
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads MODULE)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib/static)

include(GNUInstallDirs)
include(UseVanetzaExtension)
include(UseMitvane)

# Enable usage of targets' folder property (good for Visual Studio)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Find project dependencies
find_package(Cohda MODULE QUIET)
find_package(OpenSSL QUIET)
find_package(Vanetza 0.8 REQUIRED)

set(Boost_COMPONENTS date_time log log_setup) # used by VanetzaConfig generator
find_package(Boost 1.58 REQUIRED COMPONENTS ${Boost_COMPONENTS})
# query optional Boost components one by one quietly
foreach(component IN ITEMS program_options system)
  find_package(Boost 1.58 QUIET COMPONENTS ${component})
  if (Boost_${component}_FOUND)
    list(APPEND Boost_COMPONENTS ${component})
  endif()
endforeach()

add_subdirectory(vanetza-extension/asn1)
add_subdirectory(vanetza-extension/facilities)
add_subdirectory(mitvane)

if(TARGET Cohda::headers)
  add_subdirectory(cohda_proxy)
endif()

# interface library for convenience
get_property(_components GLOBAL PROPERTY VANETZA_EXTENSION_COMPONENTS)
add_library(vanetza-extension INTERFACE)
add_library(Vanetza-extension::vanetza-extension ALIAS vanetza-extension)
foreach(_component IN LISTS _components)
  target_link_libraries(vanetza-extension INTERFACE ${_component})
endforeach()
