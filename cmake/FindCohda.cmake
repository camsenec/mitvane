#  This file is copied
#  by Tomoya Tanaka <deepsky2221@gmail.com>
#  from <https://github.com/riebl/vanetza/blob/master/cmake/FindCohda.cmake>
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


find_path(COHDA_ROOT
    NAMES "ieee1609/app/lib1609/include/lib1609.h" "cohda/kernel/include/linux/cohda/llc/llc-api.h"
    PATHS "/home/duser"
    PATH_SUFFIXES mk5 mk2
    CMAKE_FIND_ROOT_PATH_BOTH
    DOC "Cohda SDK")

set(COHDA_KERNEL_INCLUDE_DIR ${COHDA_ROOT}/ieee1609/kernel/include)
set(COHDA_LIB1609_INCLUDE_DIR ${COHDA_ROOT}/ieee1609/app/lib1609/include)
set(COHDA_LLC_INCLUDE_DIR ${COHDA_ROOT}/cohda/kernel/include/linux/cohda/llc)
set(COHDA_INCLUDE_DIRS ${COHDA_KERNEL_INCLUDE_DIR} ${COHDA_LIB1609_INCLUDE_DIR} ${COHDA_LLC_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Cohda "Cohda SDK not found" COHDA_ROOT)
mark_as_advanced(COHDA_ROOT)
if(COHDA_FOUND AND NOT TARGET Cohda::headers)
    add_library(Cohda::headers UNKNOWN IMPORTED)
    set_target_properties(Cohda::headers PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${COHDA_INCLUDE_DIRS}")
endif()
