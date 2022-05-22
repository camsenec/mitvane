#  This file is modified
#  by Tomoya Tanaka <deepsky2221@gmail.com>
#  from <https://github.com/riebl/vanetza/blob/master/vanetza/facilities/CMakeLists.txt>
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
#  - Some source files are added for CXX_SOURCES.
#  - Dependent library asn1 is changed to Vanetza-extension::asn1.


set(CXX_SOURCES
    functions.cpp
    cam_functions.cpp
    denm_functions.cpp
    spatem_functions.cpp
    mapem_functions.cpp
)

add_vanetza_extension_component(facilities ${CXX_SOURCES})
target_link_libraries(facilities PRIVATE Vanetza-extension::asn1)
target_link_libraries(facilities PRIVATE Boost::date_time Vanetza::geonet)

add_test_subdirectory(tests)

