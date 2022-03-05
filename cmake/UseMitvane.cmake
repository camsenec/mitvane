#  This file is modified
#  by Tomoya Tanaka <deepsky2221@gmail.com>
#  from <https://github.com/riebl/vanetza/blob/master/cmake/UseVanetzaExtension.cmake>
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
#  - Change function names and variable names from vanetza family to mitvane family
#


#
# add_mitvane_component(<name> <sources...>)
#
function(add_mitvane_component name)
    set(sources ${ARGN})
    add_library(${name} ${sources})
    add_library(Mitvane::${name} ALIAS ${name})
    target_include_directories(${name} PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>)
    set_target_properties(${name} PROPERTIES
        OUTPUT_NAME mitvane_${name})
    set_property(GLOBAL APPEND PROPERTY MITVANE_COMPONENTS ${name})
endfunction()

#
# add_test_subdirectory(<directory>)
#
# Add subdirectory only when tests are enabled via BUILD_TESTS
#
function(add_test_subdirectory directory)
  if(BUILD_TESTS)
    add_subdirectory(${directory})
  endif()
endfunction(add_test_subdirectory)
